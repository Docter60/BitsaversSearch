# BitsaversSearch

## Acknowledgements
This project is using the [Ultralight](https://ultralig.ht/) HTML renderer and their [quick start repository](https://github.com/ultralight-ux/ultralight-quick-start).
Additional dependencies include:
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
- [asio](https://github.com/chriskohlhoff/asio) (non-boost)

## Features
- Keyword search in path and filename
- Download links are provided and open in your default browser
- Updates occur once the last update is over a day old
- Updates can be forced by deleting the LastUpdated.txt or the assets/data.json file

## Building BitsaversSearch
```shell
git clone https://github.com/Docter60/BitsaversSearch
cd BitsaversSearch
git submodule init
git submodule update
cmake -B build
cmake --build build --config Release
```

## Running BitsaversSearch

Navigate to the executable's directory and run 'BitsaversSearch' to launch the program.
