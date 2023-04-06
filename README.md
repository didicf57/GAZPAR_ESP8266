# GAZPAR_ESP8266

Alors que la consommation journalière pouvait etre récupérée et enregistrée automatiquement sur InfluxDB grace à un script analysant le site Web de GRDF,depuis fin janvier 2023, l'introduction d'un Capcha bloque desormais cet accés automatisé.

Il est donc necessaire de faire la mesure de consommation directement sur le compteur.

Un ESP8266 est donc relié au compteur et compte les Pulses disponibles sur une "sortie" tout/rien.
L'ESP devant etre alimenté par pile, il doit alors etre en trés basse consommation (Deep_Sleep) entre deux pulses.
Le dépot en Wifi sur InfluxDB peut se faire toutes les N pulses.

Une pulse est générée tous les 10 litres de gaz.
Le mode WiFi consomme 70/80mA pendant 2 secondes (suivant configuration de la connexion WiFi)
En Deep_Sleep, l'ESP consomme moins de 20µA (à condition de supprimer la LED du module)
Lors du reveil, l'ESP consomme une vingtaine de mA pendant moins de 300ms.

L'ESP82 est placé en Deep_Sleep et reveillé par un PULSE. Un compteur est incrementé. Apres N pulses comptées , une connection WiFi est lancée pour effectuer le depot InfluxDB puis retourne en Deep_Sleep.
La configuration du WiFi se fait dans le code source.

Carte electronique est en cours de réalisation.


