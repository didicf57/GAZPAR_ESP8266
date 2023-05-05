# GAZPAR_ESP8266 (sur module ESP-01)

Alors que la consommation journalière pouvait etre récupérée et enregistrée automatiquement sur InfluxDB grace à un script analysant le site Web de GRDF,depuis fin janvier 2023, l'introduction d'un Captcha bloque desormais les accés automatisés.

Il est donc necessaire de faire la mesure de consommation directement sur le compteur.

Un ESP-01 est donc relié au compteur et compte les Pulses disponibles sur une "sortie" tout/rien.

Le compteur Gazpar dispose d'une "sortie" type collecteur/drain ouvert sur un connecteur type JAE MX44 male.
Des cordons étanches femelle sont dispobles un peu partout. Des fils équipés avec des broches femelle type Dupont fonctionnent trés bien également.
La broche GND semble etre en haut, la broche Pulse en bas.

L'ESP devant etre alimenté par une batterie (Type Chlorure de lithium thionyle, type ER26500 (8.5Ah)/ ER34615 (19Ah)), il doit alors être en trés basse consommation (Deep_Sleep) entre deux pulses.
Afin de limiter la consommation (connexion WiFi), le dépot en Wifi sur InfluxDB se fait toutes les N pulses. (configurable en fonction du débit compteur)

Une pulse est générée tous les 10 litres de gaz.
Le mode WiFi consomme 70/80mA pendant 2 secondes (suivant configuration de la connexion WiFi)
En Deep_Sleep, l'ESP consomme moins de 20µA (à condition de supprimer la LED du module)
Lors du reveil, l'ESP consomme une vingtaine de mA pendant moins de 300ms.

L'ESP-01 est placé en Deep_Sleep et reveillé par un PULSE sur la pin RESET. Un compteur est incrementé. Apres N pulses comptées , une connection WiFi est lancée pour effectuer le depot InfluxDB puis retourne en Deep_Sleep.
La configuration du WiFi se fait dans le code source.

Autonomie estimée :
pour une consomation annuelle de 17500kWh (maison de 100m2 isolée avec eau chaude et 4 personnes) 
En moyenne, 1 m3 de gaz naturel équivaut à 11 kWh. (sources GRDF / Hellowatt)
--> volume total annuel : 1590 m3 soit 1 590 000 litres
A raison d'une pulse tous les 10 litres cela fait 159 000 pulses par an soit 18 pulses par heure.

Un pulse : ESP-01 conso 20mA pendand 100ms (reveil, increment compteur, sleep)

Un dépot (tout les 5 Pulses) = 80mA pendant 2 secondes (typique) soit 3.6 dépots par heure
Courant Sleep : 20µA (le reste du temps)
Autonomie : Batterie 8.5Ah --> 5 ans (typique)



Voir image pour definition des broches et cablage.


