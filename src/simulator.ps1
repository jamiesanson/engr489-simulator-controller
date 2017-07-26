param([String]$com="COM3", [Int32]$baud="115200")
Write-Host ''
Write-Host '-------------------------------------------------'
Write-Host 'ENGR489 Solar Simulator Controller - Jamie Sanson'
Write-Host '-------------------------------------------------'
Write-Host ''


$port = new-Object System.IO.Ports.SerialPort $com, $baud, None, 8, one

# Check that port exists
Try {
    $port.Open()
    $port.Close()
} 
Catch {
    Write-Host ''
    Write-Error "Failed to start serial port at $($com), check this and try again"
    Write-Host ''
    break
}

Write-Host 'Serial Started';
Write-Host -NoNewLine 'Press any key to begin self test...'
$null = $Host.UI.RawUI.ReadKey('NoEcho,IncludeKeyDown')