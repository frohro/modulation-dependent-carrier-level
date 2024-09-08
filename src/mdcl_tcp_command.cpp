// To do: Make the messages sent back and forth to the Teensy via TCP/IP shorter.
// You can make a table here with each message described carefully and then a brief 
// few characters for the message.

#include <QNEthernet.h>
#include <Arduino.h>
#include <globals.h>

using namespace qindesign::network;

// Function prototype 
void getAndProcessMDCLTCPCommand(EthernetServer &tcpServer);

void getAndProcessMDCLTCPCommand(EthernetServer &tcpServer)
{
    // Check for ethernet clients
    //#code
    //# a = setsamplesPerDecay
    //# b = getsamplesPerDecay
    //# c = setMDCL 
    //# d = getMDCL
    //# e = setAdjust
    //# f = getAdjust

    
    
    
    EthernetClient client = tcpServer.available();
    if (client)
    {
        digitalWrite(connectedPin, HIGH); // Connected to an LED to indicate connection to the Server

        // Read the incoming command from the client
        String command = client.readStringUntil('\n'); // Read until newline

        if (command.startsWith("a:"))
        {
            // Extract the integer value and update
            String valueString = command.substring(2); // Get value after "setcyclesPerOneDecay:"
            samplesPerDecay = valueString.toInt();    // Update received integer
        }
        else if (command == "b")
        {
            // Send the current integer value back to the client
            client.println(samplesPerDecay);
        }
        else if (command.startsWith("c:"))
        {
            // Extract the integer value and update; 0 = dcc, 1 = amc, 2 = other
            String valueString = command.substring(2); // Get value after "setMDCL:"
            currentMDCLNumber = valueString.toInt();   // Update received integer
        }
        else if (command == "d")
        {
            // Send the current mdcl value back to the client
            client.println(currentMDCLNumber);
        }
        else if (command.startsWith("e:"))
        {
            // Extract the integer value and update
            String valueString = command.substring(2); // Get value after "setAdjust:"
            float temp = valueString.toFloat();         // Update received float
            if (temp < .6)
                percentAdjust = .6;
            else if (temp > 1)
                percentAdjust = 1;
            else
                percentAdjust = temp;
        }
        else if (command == "f")
        {
            // Send the current adjust value back to the client
            client.println(percentAdjust);
        }
        // Close the connection
        client.stop();
    }
}