param([string]$Path = 'calc.csv', [switch]$RequirePlayedCoverage, [switch]$RequireChallengeCoverage)
$ErrorActionPreference = 'Stop'
$culture = [Globalization.CultureInfo]::InvariantCulture
function Numbers([string]$text) {
    @($text.Trim('(', ')').Split(',') | ForEach-Object { [double]::Parse($_, $culture) })
}
$rows = @(Import-Csv -LiteralPath $Path)
if ($rows.Count -eq 0) { throw 'CSV has no scene rows.' }
$ids = @{}
foreach ($row in $rows) {
    if ($row.Phase -ne '2' -or $row.Primitive -ne 'Unit Cube' -or $row.Matrix_Order -ne 'T*Rz*Ry*Rx*H*S') { throw "Incorrect pipeline metadata: $($row.Object_ID)" }
    if ($ids.ContainsKey($row.Object_ID)) { throw "Duplicate ID: $($row.Object_ID)" }
    $ids[$row.Object_ID] = $true
    $p = Numbers $row.Local_Point; $s = Numbers $row.Scale; $h = Numbers $row.Shear; $t = Numbers $row.Translation
    $x=$p[0]*$s[0]; $y=$p[1]*$s[1]; $z=$p[2]*$s[2]
    $a=$x+$h[0]*$y+$h[1]*$z; $b=$h[2]*$x+$y+$h[3]*$z; $c=$h[4]*$x+$h[5]*$y+$z
    $rx=[double]::Parse($row.Rotation_X_deg,$culture)*[Math]::PI/180
    $ry=[double]::Parse($row.Rotation_Y_deg,$culture)*[Math]::PI/180
    $rz=[double]::Parse($row.Rotation_Z_deg,$culture)*[Math]::PI/180
    $x=$a; $y=[Math]::Cos($rx)*$b-[Math]::Sin($rx)*$c; $z=[Math]::Sin($rx)*$b+[Math]::Cos($rx)*$c
    $a=[Math]::Cos($ry)*$x+[Math]::Sin($ry)*$z; $b=$y; $c=-[Math]::Sin($ry)*$x+[Math]::Cos($ry)*$z
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
Write-Output "PASS: $($rows.Count) unique Phase 2 cube rows; independently recomputed S/H/Rx/Ry/Rz/T world points and scene coverage."
if ($RequireChallengeCoverage) {
    foreach ($level in 1..7) {
        if (-not ($rows | Where-Object { $_.Level -eq $level -and $_.Object_Type -eq 'Target' })) { throw "Missing visited level: $level" }
    }
    foreach ($type in @('Bird','Human')) {
        if (-not ($rows | Where-Object Object_Type -eq $type)) { throw "Missing NPC: $type" }
    }
    Write-Output 'PASS: actual observed transforms cover Levels 1-7, birds and humans.'
}