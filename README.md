<p align="center">
    <img width="200px" height="auto" src="assets/dump.png" />
    <h3 align="center">dumper</h3>
    <p align="center"><i>This project showcases dumping kernel drivers from usermode</i></p>
</p>

## About

It is composed of 2 main components:
1. **dejavu**  
   A kernel driver that communicates with the usermode process through a shared memory page. The driver currently implements one operation:\
       - ```kread``` reads an arbitrary kernel address and returns the contents to usermode.

3. **moonwalk**  
   A usermode application that interfaces with the kernel driver to send commands and receive data.


## Support
- Windows 10 x64 22H2
- AMD/Intel


## Usage
1. Launch ```moonwalk.exe [target_driver_name]```
2. Sign and load the driver. Otherwise, use [kdmapper](https://github.com/TheCruZ/kdmapper), [KDU](https://github.com/hfiref0x/KDU)
4. Enjoy!

## Credits
- @SamuelTulach - README design
