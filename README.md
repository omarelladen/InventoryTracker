# Sistema Inteligente de Monitoramento Patrimonial (SIMPAT)

## Website
[GitHub Page](https://omarelladen.github.io/InventoryTracker/)


## Firmware

### Install requirements (Debian)
```sh
sudo apt install arduino
```
Add URL:
```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```
### Upload code
```sh
./scripts/upload.sh
```


## Server

### Install requirements (Debian)
```sh
sudo apt install python3-fastapi sqlite3
```

### Setup database
```sh
./scripts/setup_db.sh
```

### Run server
```sh
cd server
python3 main.py
```
