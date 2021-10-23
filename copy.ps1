$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt -First 1
$VERSION = Get-Content $PSScriptRoot/VERSION -First 1

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk VERSION=$VERSION
adb shell am force-stop com.beatgames.beatsaber
adb push $PSScriptRoot/libs/arm64-v8a/libbeattogether.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libbeattogether.so
adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
$timestamp = Get-Date -Format "MM-dd HH:mm:ss.fff"
$bspid = adb shell pidof com.beatgames.beatsaber
while ([string]::IsNullOrEmpty($bspid)) {
    Start-Sleep -Milliseconds 100
    $bspid = adb shell pidof com.beatgames.beatsaber
}
if ($args.Count -eq 0) {
    echo "Start logging!"
    adb logcat -T "$timestamp" --pid $bspid | Select-String -pattern "BeatTogether" 
}
if ($args[0] -eq "--file") {
    echo "Logging and saving to file!"
    (adb logcat -T "$timestamp" --pid $bspid | Select-String -pattern "(QuestHook|modloader|AndroidRuntime)") | Out-File $PSScriptRoot/logcat.log -Append
}
if ($args[0] -eq "--verbose") {
    echo "Logging and saving to file!"
    adb logcat -T "$timestamp" --pid $bspid > $PSScriptRoot/logcat.log 
}
