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

**Cahier des charges**  :
Fréquence de la PWM : 20kHz
Temps mort minimum : à voir selon la datasheet des transistors (faire valider la valeur)
Résolution minimum : 10bits.
 Ci-dessous la configuartion de notre ioc:
![cap1](https://github.com/user-attachments/assets/1b800cef-2044-4567-a1d2-a77a5364a5c8)

Configuration des valeurs du Timer :
La fréquence de notre STM est  de 170Mhz est nous souaitons avoir une fréquence de 20kHz

![cap 2](https://github.com/user-attachments/assets/ad01a50a-845e-435b-af7f-31daa2f16450)
nous avons configurer le timer comme ceci :
PSC=0
ARR=8500
Comme nous notre PWM a une la phase de montée et une phase de descente donc nous devons divisier par 2 
ARR= 8500/2=4250-1=4249
Pour le calcul du temps mort, nous souhaiton avoir 200ns, nous avons donc effectué les calcul suivant pour configurer le dead time dans l'ioc  donc : 
![cap4](https://github.com/user-attachments/assets/adbdb95c-55b6-49c8-a6b5-e4a953e5e690)

Sur l'ioc nous avons donc configuré Dead Time  :34  

Nous avons maintenant configuré le rapport cyclique a  60% , et nous avons voudrons generer les PWM decaclé d'une demi période avec une fréquence de 
kHz
Nous avons bien obtenu note Générer quatre PWM decaclé d'une demi période avec une fréquence de 
kHz  sur les bras de pont U et V pour controler le hacheur à partir du timer1

Obsevation sur l'osciloscoppe :
![cap5](https://github.com/user-attachments/assets/8cef079a-bdc2-4069-a637-fd02b125fcd9)

 **Premiers tests**

Nous avons varié les rapport cyclique de 10%, 20% et 70%
etapes effectuée dans le code : 
initialitions et demmarage du timer  des 4PWM: 
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
definition des rapport cyclique comme suit dans la boucle while : 
__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 4249 * 0.60); // Rapport de  25% du maximum
__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 4249 * 0.40); // Complémentaire
ci-dessous les observation sur l'osciloscope pour differents rapports cyclique : 
![10](https://github.com/user-attachments/assets/3ee251eb-eb45-4123-a854-493eba675278)
![30](https://github.com/user-attachments/assets/b5796960-7ad3-448e-9a05-4057eb34dd51)
![40](https://github.com/user-attachments/assets/66847914-c9cf-47dc-805b-98ddc98421ca)

## TP n°2 - Commande en boucle ouverte, mesure de Vitesse et de courant
Dans cette Nous allons :

Commander en boucle ouverte le moteur avec une accélération limitée,
Mesurer le courant aux endroits adéquat dans le montage,
Mesurer la vitesse à partir du codeur présent sur chaque moteur.

 **Commande de la vitesse**
Ajout fonctionnalités à notre projet dans le shell :

Commande start : permet de fixer le rapport cyclique à 50% (vitesse nulle) et d'activer la génération des pwm 
Commande stop : permet de désactiver la génération des PWM.
Commande speed XXXX : permet de définir le rapport cyclique à XXXX/PWM_MAX








# TP_ACTIONNEUR_MCC

**Binôme :**
- AMEUR Meziane
- Chair Mohammed Rafik

---

## Séance 1 - Commande MCC Basique

### Objectifs

- Générer 4 PWM en complémentaire décalée pour contrôler le moteur en boucle ouverte, en respectant le cahier des charges.
- Inclure un temps mort.
- Vérifier les signaux de commande à l'oscilloscope.
- Prendre en main le hacheur.
- Effectuer un premier essai de commande moteur.

### Cahier des Charges

- **Fréquence de la PWM** : 20 kHz
- **Temps mort minimum** : À valider selon la datasheet des transistors
- **Résolution minimum** : 10 bits

### Configuration IOC

Nous avons configuré notre projet dans STM32CubeMX pour générer les signaux PWM nécessaires :

![Configuration IOC](https://github.com/user-attachments/assets/1b800cef-2044-4567-a1d2-a77a5364a5c8)

### Configuration des Valeurs du Timer

Pour obtenir une PWM de fréquence 20 kHz à partir de notre STM32 fonctionnant à 170 MHz, nous avons calculé les valeurs de préscaler (PSC) et d'auto-reload (ARR) pour le Timer. Voici les étapes détaillées :

1. **Calcul de ARR** : 
   - Avec une fréquence de 20 kHz souhaitée, la période d'une PWM est de 50 µs (1 / 20 kHz).
   - Comme notre STM32 fonctionne à 170 MHz, nous avons une période de 5,88 ns pour chaque cycle.
   - Pour atteindre 50 µs, nous avons besoin de :  
     \( \text{ARR initial} = \frac{50 \text{ µs}}{5,88 \text{ ns}} = 8500 \).
   - Cependant, puisque la PWM possède une phase de montée et une phase de descente, nous devons diviser par 2 pour respecter cette montée et descente dans chaque période complète.  
     \( \text{ARR final} = \frac{8500}{2} - 1 = 4249 \).

2. **Configuration dans le Timer** :
   - **PSC** = 0
   - **ARR** = 4249 (pour générer une fréquence de 20 kHz)

![Configuration Timer](https://github.com/user-attachments/assets/ad01a50a-845e-435b-af7f-31daa2f16450)

3. **Calcul du Temps Mort** : Nous avons configuré le temps mort à 200 ns dans l'IOC, en ajustant la valeur `Dead Time` à 34.

### Configuration du Rapport Cyclique

Pour les essais initiaux, nous avons configuré le rapport cyclique à 60 % et généré les PWM décalés d'une demi-période avec une fréquence de 20 kHz sur les bras de pont U et V pour contrôler le hacheur via Timer1.

#### Observation à l'Oscilloscope

Voici les signaux PWM observés à l'oscilloscope pour vérifier la configuration :

![Oscilloscope Capture](https://github.com/user-attachments/assets/8cef079a-bdc2-4069-a637-fd02b125fcd9)

### Premiers Tests

Nous avons testé plusieurs rapports cycliques : 10 %, 20 % et 70 %.

#### Étapes dans le Code

1. **Initialisation et démarrage du timer pour les 4 PWM** :
   ```c
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
   HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
   HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
```
### Définition des Rapports Cycliques dans la Boucle `while`

```c
__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 4249 * 0.60); // Rapport de 60%
__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 4249 * 0.40); // Complémentaire
```

Observations à l'Oscilloscope pour Différents Rapports Cycliques
Rapport cyclique 10 %

![10](https://github.com/user-attachments/assets/3ee251eb-eb45-4123-a854-493eba675278)

Rapport cyclique 30 %
![30](https://github.com/user-attachments/assets/b5796960-7ad3-448e-9a05-4057eb34dd51)


Rapport cyclique 40 %
![40](https://github.com/user-attachments/assets/66847914-c9cf-47dc-805b-98ddc98421ca)


TP n°2 - Commande en Boucle Ouverte, Mesure de Vitesse et de Courant
Objectifs
Commande en boucle ouverte avec une accélération limitée.
Mesure du courant aux endroits adéquats dans le montage.
Mesure de la vitesse à partir du codeur présent sur chaque moteur.
Commande de la Vitesse
Ajout de fonctionnalités dans le shell :

Commande start : Fixe le rapport cyclique à 50 % (vitesse nulle) et active la génération des PWM.
Commande stop : Désactive la génération des PWM.
Commande speed XXXX : Définit le rapport cyclique à XXXX / PWM_MAX.

