# StationMeteoInterieur

Projet d'une station méteo permettant de surveiller la température, l'humidité, le taux de CO2 ainsi que le total de particules nocives TVOC. Il permet également de détecter les fumées.
Toutes ces informations sont indiquées sur un écran LCD et des leds informent l'utilisateur d'un taux trop élévés de particules, de CO2 ou de fumée.

### Matériel necéssaire:

- Capteurs :
  - DHT22 (température, humidité)
  - CSS811 (CO2, TVOC)
  - MQ-2 (fumées)
  
- Un écran LCD 4 lignes, 20 caractères en I2C

- Arduino : Nano BLE ou Nano 33 IOT

- Leds 5mm : rouge, verte, bleu

- Transistors : 2 BSS138
- Résistances CMS (0805 imperial) : 3x100Ohms, 4x10kOhms
- Résistance tranversante : 10kOhms
- Condensateurs tranversants : 100uF

- Connecteurs : PinSockets pas de 2.54mm de diverses longueurs
