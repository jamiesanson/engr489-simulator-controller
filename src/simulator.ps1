param([string]$ComPort="COM3", [Int32]$Baud="115200")
Write-Host ''
Write-Host '-------------------------------------------------'
Write-Host 'ENGR489 Solar Simulator Controller - Jamie Sanson'
Write-Host '-------------------------------------------------'
Write-Host ''


$port = new-Object System.IO.Ports.SerialPort $ComPort, $Baud, None, 8, one

# Check that port exists
Try {
    $port.Open()
    $port.Close()
} 
Catch {
    # Fail the script if port exists
    Write-Host ''
    Write-Error "Failed to start serial port at $($ComPort), check this and try again"
    Write-Host ''
    break
}

Write-Host 'Serial Started'
Write-Host ''

if ($psISE)
{
    Add-Type -AssemblyName System.Windows.Forms
    [System.Windows.Forms.MessageBox]::Show("Press any key to begin self test...")
}
else
{
    Write-Host "Press any key to begin self test..." -ForegroundColor Yellow
    $null = $host.ui.RawUI.ReadKey("NoEcho,IncludeKeyDown")
}

$port.Open();
$port.WriteLine('test');
Write-Host $port.ReadLine()
$port.Close();