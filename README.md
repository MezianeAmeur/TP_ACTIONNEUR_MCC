# TP_ACTIONNEUR_MCC

**Binôme :**
- AMEUR Meziane
- Chair Mohammed Rafik


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
   - Comme notre STM32 fonctionne à 170 MHz, chaque cycle a une période de 5,88 ns.
   - Pour atteindre une période totale de 50 µs, nous avons besoin de calculer la valeur initiale de `ARR` :

     ARR initial = 50 µs / 5,88 ns = 8500

 **Comptage Up-Down**

Dans notre configuration initiale, nous avions défini le mode PWM en comptage **Up** uniquement, avec un registre d'auto-rechargement (ARR) fixé à 8499. Cependant, en passant au mode **Up-Down** (comptage en montée et descente), il est nécessaire d’adapter la configuration pour tenir compte du cycle double du compteur.

En mode Up-Down :  
- Le compteur monte jusqu’à ARR, puis redescend à 0.  
- Pour conserver la même fréquence PWM qu’en mode Up, **ARR est divisé par 2** afin que la période totale (montée + descente) reste identique.
Le graphe ci-dessous illustre le fonctionnement du mode PWM, montrant comment la valeur de CCR1 détermine le rapport cyclique (duty cycle) en fonction de son intersection avec le compteur.

  ![cap1](https://github.com/user-attachments/assets/55f2fe8d-e6ce-4ebf-a56c-d87249612708)
Dans ce contexte :  
- **ARR** définit la plage maximale du compteur.  
- **CCR1** fixe le moment où le signal passe de haut à bas (ou inversement), contrôlant ainsi la largeur de l’impulsion.  
- Le passage au mode Up-Down modifie la dynamique du comptage, mais le principe de génération de PWM reste inchangé.

     ARR final = (8500 / 2) - 1 = **4249**

2. **Configuration dans le Timer** :
 Choix du timer 1 en mode :

Channel 1 -> **PWM Generation CH1 CH1N**
Channel 2 -> **PWM Generation CH2 CH2N**

Pour obtenir une fréquence de 20 kHz tout en respectant la résolution et le rapport cyclique, nous configurons les réglages suivants :

   - **PSC** = 0 => 170 000 000 Hz
   - **ARR** = 4249 
   - **Counter Mode** = Center Aligned Mode1
   - **CCR1** = 2550
   - **CCR2** = ARR-CCR1 = 4250-2550 = 1700

![cap6](https://github.com/user-attachments/assets/9eb6e188-2ab6-41b1-832f-0ad3e92de98b)

3. **Calcul du Temps Mort** :
 Selon la datasheet du transistor, le Rise Time et le Fall Time sont à 35 ns, soient des temps morts à 70 ns et pourour être large, nous avons choisi de prendre un temps morts à 200 ns.
 Nous avons alors configuré la valeur `Dead Time` à 34., ci-dessous le calcul effectué pour l'obtention de cette valeur:
   
![cap4](https://github.com/user-attachments/assets/adbdb95c-55b6-49c8-a6b5-e4a953e5e690)

### Configuration du Rapport Cyclique

Pour les essais initiaux, nous avons configuré le rapport cyclique à 60 % et généré les PWM décalés d'une demi-période avec une fréquence de 20 kHz sur les bras de pont U et V pour contrôler le hacheur via Timer1.

#### Observation à l'Oscilloscope

Voici les signaux PWM observés à l'oscilloscope pour vérifier la configuration :

![Oscilloscope Capture](https://github.com/user-attachments/assets/8cef079a-bdc2-4069-a637-fd02b125fcd9)

La fréquence est bien de 20 kHz

### Premiers Tests

Nous avons testé plusieurs rapports cycliques 

#### Étapes dans le Code

1. **Initialisation et démarrage du timer pour les 4 PWM** :

   ```c 
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
   HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
   HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

2. **Définition des Rapports Cycliques dans la Boucle `while`**

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


## TP n°2 - Commande en Boucle Ouverte, Mesure de Vitesse et de Courant
Objectifs
Commande en boucle ouverte avec une accélération limitée.
Mesure du courant aux endroits adéquats dans le montage.
Mesure de la vitesse à partir du codeur présent sur chaque moteur.
Commande de la Vitesse

Ajout de fonctionnalités dans le shell :

- **Commande start** : Fixe le rapport cyclique à 50 % (vitesse nulle) et active la génération des PWM.
- **Commande stop** : Désactive la génération des PWM.
- **Commande speed XXXX** : Définit le rapport cyclique à XXXX / PWM_MAX.

On les ajoute au shell.c, où on a codé la commande speed comme suit : 
 ```c
else if(strcmp(argv[0],"speed")==0){
			float speedVal=atoi(argv[1]);
			if (speedVal> PWM_MAX){
				speedVal=PWM_MAX;
			}
			if (speedVal< PWM_MIN){
				speedVal=PWM_MIN;
			}

			speedVal=(DUTY_MAX*speedVal)/100; //PWM en pourcentage
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,speedVal);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,DUTY_MAX - speedVal);



		}
		/*
		 * start -> Activation des PWM avec un rapport cyclique de 50%
		 * stop  -> Désactivation des PWM
		 */


		else if(strcmp(argv[0],"start")==0){

			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_1,0.5*DUTY_MAX);
			__HAL_TIM_SetCompare(&htim1,TIM_CHANNEL_2,0.5*DUTY_MAX);
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
			//HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
			//HAL_TIM_Base_Start_IT(&htim5);

		}
		else if(strcmp(argv[0],"stop")==0){
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
		}

```


