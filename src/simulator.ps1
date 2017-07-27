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
    # Fail the script if port doesn't open
    Write-Host ''
    Write-Error "Failed to start serial port at $($ComPort), check this and try again"
    Write-Host ''
    break
}

Write-Host 'Serial Started'
Write-Host ''

Try {
    Write-Host "Press any key to begin self test..." -ForegroundColor Yellow
    $null = $host.ui.RawUI.ReadKey("NoEcho,IncludeKeyDown")
}
Catch {
    # Ignore exceptions here as this only occurs in PowerShell Integrated Console
}

Write-Host ''
Write-Host 'Begining self test:'

$port.Open();
$port.WriteLine('test');

$rec = $port.ReadLine()

# Read debug messages until success response
while ($rec -notlike "done*") {
    # If test fails, end script
    if ($rec -like "fail*") {
        $err = $port.ReadLine()
        Write-Host "Device test failed. Error: $($err)" -ForegroundColor Red
        return
    }

    # Log to host and wait for next
    Write-Host $rec
    $rec = $port.ReadLine()    
}
$port.Close();

Write-Host "Test Complete"

Write-Host ''

Try {
    Write-Host "Press any key to begin simulator run..." -ForegroundColor Yellow
    $null = $host.ui.RawUI.ReadKey("NoEcho,IncludeKeyDown")
}
Catch {
    # Ignore exceptions here as this only occurs in PowerShell Integrated Console
}

Write-Host ''
Write-Host 'Begining main run:'

$port.Open();
$port.WriteLine('run');

$rec = $port.ReadLine()

# Read debug messages until success response
while ($rec -notlike "done*") {
    # If test fails, end script
    if ($rec -like "fail*") {
        $err = $port.ReadLine()
        Write-Host "Device test failed. Error: $($err)" -ForegroundColor Red
        return
    }

    # Log to host and wait for next
    Write-Host $rec
    $rec = $port.ReadLine()    
}

$port.Close();

Write-Host ''
Write-Host ''
Write-Host "Run successful"
Write-Host "Please remove the micro-SD card from the Teensy to collect CSV-formatted data"
Write-Host ''

