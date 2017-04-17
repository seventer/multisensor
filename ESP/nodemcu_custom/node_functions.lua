


--
-- wifi.sta.getap(listap) (for debugging only, not used in normal operation)
--
function listap(t)
      for k,v in pairs(t) do
        print(k.." : "..v)
      end
end

--
-- connectAP("MySSID","AP-Password",10)
--
function connectAP(SSID,PASW,TIMEO)
    retry=0
    wifi.setmode(wifi.STATION)
    wifi.sta.config(SSID,PASW)
    wifi.sta.connect()
    tmr.alarm(0, 1000, 1, function()
        if (wifi.sta.getip() == nil)  then
            --print("Busy...")
        else
            print('Connected')
            tmr.stop(0)
        end
        retry = retry + 1
        if (retry>TIMEO) then
            print('Failed')
            tmr.stop(0)
        end
    end)         
end

--
-- getIP()
--
function getIP()
    ip = wifi.sta.getip()
    print(ip)
end

--
-- Get RSSI Wifi strength in percentage
--
function getRSSI()
    retVal=0
    if (wifi.sta.status()==5) then
               rssi = wifi.sta.getrssi()
               if (rssi >= -50) then 
                    retVal=100
               else
                    if (rssi <= -100) then
                         retVal=0
                    else 
                         retVal = 2 * (rssi + 100)
                    end
               end
     else
          retVal = 0
     end
     print(retVal)    
end



--
-- /json.htm?type=command&param=getlightswitches
--
function sendData(sHost,iPort,sUri)
     conn = nil
     conn = net.createConnection(net.TCP, 0)
     conn:on("receive", function(sck, c) print(c) end)
     conn:on("connection",
          function(conn, payload)
               --print("Connected")
               conn:send("GET ".. sUri .. " HTTP/1.1\r\n"
                    .."Host: ".. sHost .. ":" .. iPort .. "\r\n"
                    .."Connection: close\r\n"
                    .."User-Agent: Mozilla/4.0 (compatible; esp8266 GvS; Windows NT 5.1)\r\n"
                    .."Accept: */*\r\n\r\n") 
           end)          
         -- conn:on("disconnection", function(conn, payload) print('Disconnected') end)
          conn:connect(iPort,sHost)
end



-- Print welcome message after loading this module
print("Ready")

