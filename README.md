# MonitoringTool

A tool to monitor servers availability via TCP, UDP and ICMP.

## Development

This is a win desktop application written on _C++_ with _Qt Framework_.

### Requirements:
- [Microsoft Visual Studio 2017](https://visualstudio.microsoft.com/en/vs/older-downloads/)
- [Qt 5.12.11](https://download.qt.io/archive/qt/5.12/5.12.11/)
- [Qt Visual Studio Tools 2017](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123)

### Installing for development
1. Install MSVC 2017
2. Install Qt 5.12.11
3. Install Qt Visual Studio Tools
4. Connect Qt to MSVC
    - Open MonitoringTool project in MSVC
    - Go to menu `Qt VS Tools`->`Qt Versions`
    - Add your Qt version: give it a name and set path to `%QT_DIR%\5.12.11\msvc2017_64`
5. Build project

#### Qt Modules
Qt modules used in the project:
- _core_
- _gui_
- _widgets_
- _network_

### Features
App is looking for `radhud.ini` file at launch and will exit if it won't found any.
`radhud.ini` contains:
- _colors_ section for setting colors palette
- _polling_ section with servers availability frequency and server status sensitivity parameters
- _resources_ section with the list of servers to monitor.  

Unlike first two sections having ini-file format the latter have a format `<server_friendly_name> <server_address> <server_port>`

There are three types of protocols would be used according to port value:
- TCP protocol for usual ports like `80`, `443` etc.
- UDP protocol for ports with asterisk (e.g. `80*`, `333*`)
- ICMP protocol if there is an 'ICMP' value in port parameter


Example of `radhud.ini`:

```[colors]
backgroundColor=cccccc
greenColor=5bb749
yellowColor=fce23a
redColor=d6415f

[polling]
frequencySeconds=6
sensitivity=3

[resources]
example1   example.com	80
google     google.com   80*
localhost  127.0.0.1    ICMP
```
