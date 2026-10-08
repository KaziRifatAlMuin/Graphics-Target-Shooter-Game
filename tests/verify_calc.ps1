param([string]$Path = 'calc.csv', [switch]$RequirePlayedCoverage, [switch]$RequireChallengeCoverage, [switch]$RequireModeCoverage)
$ErrorActionPreference = 'Stop'
$culture = [Globalization.CultureInfo]::InvariantCulture
# Parse an XYZ/shear tuple using a fixed decimal-point culture for reproducible comparisons.
function Numbers([string]$text) {
    @($text.Trim('(', ')').Split(',') | ForEach-Object { [double]::Parse($_, $culture) })
}
$rows = @(Import-Csv -LiteralPath $Path)
if ($rows.Count -eq 0) { throw 'CSV has no scene rows.' }
$ids = @{}
# Independently recompute each exported corner in S -> H -> Rx -> Ry -> Rz -> T order.
foreach ($row in $rows) {
    if ($row.Phase -ne '4' -or $row.Primitive -ne 'Unit Cube' -or $row.Matrix_Order -ne 'T*Rz*Ry*Rx*H*S') { throw "Incorrect pipeline metadata: $($row.Object_ID)" }
    if (-not $row.Parent_or_Group -or -not $row.Color_RGB -or $row.Snapshot_State -notin @('Current','Last observed') -or -not $row.Observed_Time_seconds) {
        throw "Missing assembly/material/observation metadata: $($row.Object_ID)"
    }
    if ($ids.ContainsKey($row.Object_ID)) { throw "Duplicate ID: $($row.Object_ID)" }
    $ids[$row.Object_ID] = $true
    $p = Numbers $row.Local_Point; $s = Numbers $row.Scale; $h = Numbers $row.Shear; $t = Numbers $row.Translation
    # Scale: (x,y,z)=(px*sx,py*sy,pz*sz).
    $x=$p[0]*$s[0]; $y=$p[1]*$s[1]; $z=$p[2]*$s[2]
    # Shear: x'=x+hxy*y+hxz*z, y'=hyx*x+y+hyz*z, z'=hzx*x+hzy*y+z.
    $a=$x+$h[0]*$y+$h[1]*$z; $b=$h[2]*$x+$y+$h[3]*$z; $c=$h[4]*$x+$h[5]*$y+$z
    # Convert all rotation angles with radians=degrees*pi/180 before calling sine/cosine.
    $rx=[double]::Parse($row.Rotation_X_deg,$culture)*[Math]::PI/180
    $ry=[double]::Parse($row.Rotation_Y_deg,$culture)*[Math]::PI/180
    $rz=[double]::Parse($row.Rotation_Z_deg,$culture)*[Math]::PI/180
    # Rotate X: y'=cos(rx)*y-sin(rx)*z, z'=sin(rx)*y+cos(rx)*z.
    $x=$a; $y=[Math]::Cos($rx)*$b-[Math]::Sin($rx)*$c; $z=[Math]::Sin($rx)*$b+[Math]::Cos($rx)*$c
    # Rotate Y: x'=cos(ry)*x+sin(ry)*z, z'=-sin(ry)*x+cos(ry)*z.
    $a=[Math]::Cos($ry)*$x+[Math]::Sin($ry)*$z; $b=$y; $c=-[Math]::Sin($ry)*$x+[Math]::Cos($ry)*$z
    # Rotate Z, then translate: X=cos(rz)*x-sin(rz)*y+tx, Y=sin(rz)*x+cos(rz)*y+ty, Z=z+tz.
    $expected=@(([Math]::Cos($rz)*$a-[Math]::Sin($rz)*$b+$t[0]),([Math]::Sin($rz)*$a+[Math]::Cos($rz)*$b+$t[1]),($c+$t[2]))
    $actual=Numbers $row.Result_World_Point
    for ($axis=0; $axis -lt 3; $axis++) {
        if ([Math]::Abs($expected[$axis]-$actual[$axis]) -gt 0.0001) { throw "World point mismatch: $($row.Object_ID) axis $axis" }
    }
}
foreach ($category in @('Arena','Boundary','Cargo','Target','Weapon','Lighting','Environment')) {
    if (-not ($rows | Where-Object Object_Type -eq $category)) { throw "Missing category: $category" }
}
if ($RequirePlayedCoverage) {
    foreach ($id in @('PISTOL_BODY','SHOTGUN_BODY','RIFLE_BODY')) { if (-not $ids.ContainsKey($id)) { throw "Missing played weapon: $id" } }
    foreach ($weapon in @('PISTOL','SHOTGUN','ASSAULT RIFLE')) {
        if (-not ($rows | Where-Object { $_.Object_ID -like 'PROJECTILE_*' -and $_.Component -eq $weapon })) { throw "Missing observed projectile: $weapon" }
    }
}
Write-Output "PASS: $($rows.Count) unique Phase 4 cube rows; independently recomputed S/H/Rx/Ry/Rz/T world points and scene coverage."
if ($RequireChallengeCoverage) {
    foreach ($level in 1..7) {
        if (-not ($rows | Where-Object { $_.Level -eq $level -and $_.Object_Type -eq 'Target' })) { throw "Missing visited level: $level" }
    }
    foreach ($type in @('Bird','Human')) {
        if (-not ($rows | Where-Object Object_Type -eq $type)) { throw "Missing NPC: $type" }
    }
    Write-Output 'PASS: actual observed transforms cover Levels 1-7, birds and humans.'
}
if ($RequireModeCoverage) {
    foreach ($mode in @('Challenge','Free','Developer','BirdsEye')) {
        if (-not ($rows | Where-Object Mode -eq $mode)) { throw "Missing observed mode: $mode" }
    }
    if (-not ($rows | Where-Object { $_.Mode -eq 'BirdsEye' -and $_.Object_Type -eq 'Camera reference' })) { throw 'Missing actual observation camera marker.' }
    Write-Output 'PASS: actual observed transforms cover all modes and observation camera reference cubes.'
}
