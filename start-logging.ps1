#adb shell am force-stop com.beatgames.beatsaber
#adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
$timestamp = Get-Date -Format "MM-dd HH:mm:ss.fff"
$bspid = adb shell pidof com.beatgames.beatsaber
while ([string]::IsNullOrEmpty($bspid)) {
    Start-Sleep -Milliseconds 100
    $bspid = adb shell pidof com.beatgames.beatsaber
}
if ($args.Count -eq 0) {
    echo "Start logging!"
    adb logcat -T "$timestamp" | Select-String -pattern "(QuestHook|modloader|AndroidRuntime)"
}
if ($args[0] -eq "--file") {
    echo "Logging and saving to file!"
    (adb logcat -T "$timestamp" --pid $bspid | Select-String -pattern "(QuestHook|modloader|AndroidRuntime)") | Out-File $PSScriptRoot/logcat.log -Append
}
if ($args[0] -eq "--verbose") {
    echo "Logging and saving to file!"
    adb logcat -T "$timestamp" --pid $bspid > $PSScriptRoot/logcat.log 
}
