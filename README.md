# FYP_Final_code


# 1_kit_code 
    Upload "Kit_with_esp8266" code in arduino uno (My signal Kit).
    
# 2_Clients_server
    a)  Upload server code in esp8266 which is connected with esp32
    b)  Upload patient_x in esp8266 which is connected with Mysignal kit via Serial communication pins.
    
# 3_Firebase_Client
    Upload .ino file in esp32 but don't forget to use your credentials.
    
    
 # Work flow: 
              My signal kit read data of patient vitals (code_1) and transmit it to esp32 using esp8266 local                 area network (code 2).Esp8266 server connected with esp32 send all data to esp32 via TX pin                     (don't forget to common there ground), esp32 will
              upload that data to firbase(code_3).
