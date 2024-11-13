# TP_ACTIONNEUR_MCC
Binome: 

AMEUR Meziane
Chair Mohammed Rafik

## Séance 1 - Commande MCC basique
# Commande de MCC - niveau basique

Objectifs :

Générer 4 PWM en complémentaire décalée pour contrôler en boucle ouverte le moteur en respectant le cahier des charges,
Inclure le temps mort,
Vérifier les signaux de commande à l'oscilloscope,
Prendre en main le hacheur,
Faire un premier essai de commande moteur.

Cahier des charges :
Fréquence de la PWM : 20kHz
Temps mort minimum : à voir selon la datasheet des transistors (faire valider la valeur)
Résolution minimum : 10bits.

![cap1](https://github.com/user-attachments/assets/1b800cef-2044-4567-a1d2-a77a5364a5c8)

Configuration des valeurs du Timer 

![cap 2](https://github.com/user-attachments/assets/ad01a50a-845e-435b-af7f-31daa2f16450)
nous avons configurer le timer comme ceci :
PSC=0
ARR=8500(2)=4249-1

Nous voulons avoir un temps mort de 200ns  donc : 
![cap4](https://github.com/user-attachments/assets/adbdb95c-55b6-49c8-a6b5-e4a953e5e690)

Sur l'ioc nous avons configuré Dead Time  :34  

Montage : 
![CAP3](https://github.com/user-attachments/assets/e2cc5073-b99c-4e06-a32b-83422aa8b766)

Nous avons maintenant configuré le rapport cyclique a  60% , et nous avons obtrenu les PWM decaclé d'une demi période avec une fréquence de 
kHz
Obsevation sur l'osciloscoppe :
![cap5](https://github.com/user-attachments/assets/8cef079a-bdc2-4069-a637-fd02b125fcd9)




