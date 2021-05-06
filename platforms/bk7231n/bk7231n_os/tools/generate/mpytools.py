import os
import sys
import json


out_json = {
    "magic": "RT-Thread",
    "version": "0.1",
    "count": 4,
    "section": [
        {
            "firmware": "bk7231n_bootloader_enc.bin",
            "version": "1.00",
            "partition": "bootloader",
            "start_addr": "0x00000000",
            "size": "68K"
        },
        {
            "firmware": "bk7231_common_1.0.1_enc.bin",
            "version": "1.00",
            "partition": "app",
            "start_addr": "0x00011000",
            "size": "1150832"
        }
    ]
}


bootloader_str = sys.argv[1]
firmware_str = sys.argv[2]
#ver_str = sys.argv[2]
out_path = "config.json"

out_json["section"][0]["firmware"] = bootloader_str
out_json["section"][1]["firmware"] = firmware_str
out_json = json.dumps(out_json, sort_keys=True, indent=4)

print(out_json)

with open(str(out_path), "w") as f:
    f.write(out_json)

    
#os.system("NUM_PY=123")
