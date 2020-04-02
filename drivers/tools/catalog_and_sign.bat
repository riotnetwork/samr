"C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x86\inf2cat" /v /driver:%~dp0 /os:XP_X86,Vista_X86,Vista_X64,7_X86,7_X64,8_X86,8_X64,6_3_X86,6_3_X64,10_X86,10_X64
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x86\signtool" sign /v /ac "your-cross-cert.crt" /n "RIOT network" /tr http://timestamp.globalsign.com/?signature=sha2 /td sha256 *.cat
pause