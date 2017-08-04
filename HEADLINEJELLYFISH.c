#include <Charlieplex.h>
#include <EEPROM.h>

/*
Global defs
*/

#define DEBUG 0

#define R 1
#define Y 2
#define G 3
#define T 4
#define B 5
#define M 6
#define W 7
#define N 8

#define BA 1
#define BB 4
#define BU 2
#define BD 3

#define PA 4
#define PU A0
#define PB 2
#define PD A1

byte pins[] = {13,12,11,10,6,7,8,9,A5,A4,A3,A2};

Charlieplex charlieplex = Charlieplex(pins,12);

charliePin led1 = {2 , 3};
charliePin led2 = {2 , 1};
charliePin led3 = {2 , 0};
charliePin led4 = {0 , 2};
charliePin led5 = {0 , 3};
charliePin led6 = {0 , 1};
charliePin led7 = {1 , 3};
charliePin led8 = {1 , 0};
charliePin led9 = {1 , 2};
charliePin led10 = {3 , 2};
charliePin led11 = {3 , 1};
charliePin led12 = {3 , 0};
charliePin led13 = {4 , 7};
charliePin led14 = {4 , 5};
charliePin led15 = {4 , 6};
charliePin led17 = {6 , 7};
charliePin led16 = {6 , 5};
charliePin led18 = {6 , 4};
charliePin led19 = {7 , 5};
charliePin led20 = {7 , 4};
charliePin led21 = {7 , 6};
charliePin led22 = {5 , 4};
charliePin led23 = {5 , 6};
charliePin led24 = {5 , 7};
charliePin led25 = {10 , 8};
charliePin led26 = {10 , 9};
charliePin led27 = {10 , 11};
charliePin led28 = {11 , 8};
charliePin led29 = {11 , 10};
charliePin led30 = {11 , 9};
charliePin led31 = {9 , 11};
charliePin led32 = {9 , 10};
charliePin led33 = {9 , 8};
charliePin led34 = {8 , 10};
charliePin led35 = {8 , 11};
charliePin led36 = {8 , 9};

boolean singleOn = true;
int overall_score = 0;
int current_level = 0;
byte unlocked[19];
int animsel = 0;
void (*modPtrs[19])(int color1, int color2, int color3);
int anim_board[12] = {N,N,N,N,N,N,N,N,N,N,N,N};
int game_board[12] = {N,N,N,N,N,N,N,N,N,N,N,N};
int brightness = 500;

/*
Setup and loop
*/

void setup(){
	randomSeed(analogRead(A6));
	pinMode(PA, INPUT);
	pinMode(PB, INPUT);
	pinMode(PU, INPUT);
	pinMode(PD, INPUT);
	modPtrs[0] = chaser;
	modPtrs[1] = poles;
	modPtrs[2] = rotate;
	modPtrs[3] = opchaser;
	modPtrs[4] = rainbow;
	modPtrs[5] = police;
	modPtrs[6] = fakepwmslow;
	modPtrs[7] = dualchaser;
	modPtrs[8] = pacman;
	modPtrs[9] = fakepwmchaser;
	modPtrs[10] = fill;
	modPtrs[11] = cylon;
	modPtrs[12] = fakepwmpulse;
	if (EEPROM.read(25) == byte(25) &&
		EEPROM.read(26) == byte(26) &&
		EEPROM.read(27) == byte(27) &&
		EEPROM.read(28) == byte(28) ){
		loadScore();
	} else {
		initializeEeprom();
		startup_led_test();
		loadScore();
	}
}


void loop() {
	int die = 0;
	int skip = 0;
	unsigned int stop = 0;
	if (singleOn) {
		charlieplex.clear();
	}
	if (checkButtons() > 0) {
		if (digitalRead(PA) == LOW) {
			while (digitalRead(PA) == LOW) {
				if (digitalRead(PU) == LOW) {
					while (digitalRead(PB) != LOW) {
						if (digitalRead(PU) == LOW) {
							if (animsel > current_level) {
								animsel = 0;
								buttonBounce();
							} else {
								animsel++;
								buttonBounce();
							}
						} else if (digitalRead(PD) == LOW) {
							if (animsel == 0) {
								animsel = current_level;
								buttonBounce();
							} else {
								animsel--;
								buttonBounce();
							}
						}
						playAnimation(animsel);
					}
					saveScore();
					buttonBounce();
					skip++;
				} else if (digitalRead(PD) == LOW) {
					buttonBounce();
					charlieplex.clear();
					updateScore(0);
					buttonBounce();
					skip++;
				}
			}
			charlieplex.clear();
			buttonBounce();
			if (skip < 1) {
				games();
			}
		} else if (digitalRead(PB) == LOW) {
			while (digitalRead(PB) == LOW) {
				if (digitalRead(PA) == LOW) {
					die = 0;
					while (digitalRead(PA) == LOW && digitalRead(PB) == LOW) {
						die++;
						delay(1);
						if (die > 9999) {
							overall_score = 0;
							current_level = 0;
							animsel = 0;
							brightness = 500;
							overall_score = 0;
							current_level = 0;
							animsel = 0;
							brightness = 500;
							EEPROM.write(25,0);
							EEPROM.write(26,0);
							EEPROM.write(27,0);
							EEPROM.write(28,0);
							EEPROM.write(0,0);
							EEPROM.write(1,0);
							EEPROM.write(21,0);
							EEPROM.write(22,0);
							EEPROM.write(32,0);
							EEPROM.write(33,0);
							setup();
							loop();
						}
					}
				} else if (digitalRead(PB) == LOW && (digitalRead(PU) == LOW || digitalRead(PD) == LOW)) {
					controlBrightness();
					buttonBounce();
				}
			}
		}
	}
	playAnimation(255);
	stop = random(10000,20000);
	while (stop > 1) {
		if (checkButtons() > 0) {
			break;
		}
		delay(1);
		stop--;
	}
}

/*
Button Functions
*/

int checkButtons() {
  if (digitalRead(PA) == LOW) {
    return 1;
  }

  if (digitalRead(PB) == LOW) {
    return 4;
  }

  if (digitalRead(PU) == LOW) {
    return 2;
  }

  if (digitalRead(PD) == LOW) {
    return 3;
  }
  return 0;
}

void buttonBounce() {
	while (digitalRead(PA) == LOW || digitalRead(PB) == LOW || 
		   digitalRead(PU) == LOW || digitalRead(PD) == LOW) {
		delay(1);
	}
	return;
}

int buttonHold(int button) {
	int timer = 0;
	while (digitalRead(button) == LOW) {
		timer++;
		delay(1);
	}
	return timer;
}

int buttonTime(int timer, int button) {
	for (int i = 0; i < timer; i++) {
		if (digitalRead(button) != LOW) {
			return i;
		}
		delay(1);
	}
	return timer + 1;
}

/*
Brightness Controls
*/

void controlBrightness() {
	while (digitalRead(PB) == LOW) {
		for (int i = 1; i < 13; i++) {
			blink(W,i,1);
			if (digitalRead(PD) == LOW) {
				buttonHold(PD);
				setBrightness(1);
			} else if (digitalRead(PU) == LOW) {
				buttonHold(PU);
				setBrightness(0);
			}
		}
	}
	saveScore();
}

void setBrightness(int i) {
	if (i > 0) {
		brightness += 50;
		if (brightness > 975) {
			brightness = 50;
		}
	} else {
		brightness -= 50;
		if (brightness < 50) {
			brightness = 975;
		}
	}
}

int getBrightness() {
	return brightness;
}

/*
Scoring functions
*/

void initializeEeprom() {
	if (EEPROM.read(25) == byte(25) && 
		EEPROM.read(26) == byte(26) && 
		EEPROM.read(27) == byte(27) && 
		EEPROM.read(28) == byte(28)) {
		return;
	} else {
		for (int i = 0 ; i < 40 ; i++) {
			EEPROM.write(i, 0);
		}
		EEPROM.write(25, byte(25));
		EEPROM.write(26, byte(26));
		EEPROM.write(27, byte(27));
		EEPROM.write(28, byte(28));
	}
}

void loadScore() {
	overall_score = word(EEPROM.read(0), EEPROM.read(1));
	animsel = word(EEPROM.read(21), EEPROM.read(22));
	brightness = word(EEPROM.read(30), EEPROM.read(31));
	for (int i = 0; i < 20; i++) {
		unlocked[i] = EEPROM.read(i + 2);
	}
	current_level = word(EEPROM.read(32), EEPROM.read(33));
	overall_score = (current_level * 144) + 1;
	setLevel();
}

void eupdate(int index, byte in) {
	if (EEPROM.read(index) != in) {
		EEPROM.write(index, in);
	}
	return;
}

void saveScore() {
	eupdate(0, highByte(overall_score));
	eupdate(1, lowByte(overall_score));
	for (int i = 2; i < 20; i++) {
		if (unlocked[i - 2] > EEPROM.read(i)) {
			eupdate(i, unlocked[i - 2]);
		}
	}
	eupdate(21,highByte(animsel));
	eupdate(22,lowByte(animsel));
	eupdate(30,highByte(brightness));
	eupdate(31,lowByte(brightness));
	eupdate(32,highByte(current_level));
	eupdate(33,lowByte(current_level));
	return;
}

void updateScore(int points) {
	overall_score += points;
	dispaly_test_overall_score_fill(overall_score);
	if (current_level < 12) {
		displayScore_progressiveFill();
	}
	setLevel();
	if (overall_score < 0) {
		overall_score = (current_level * 144) + 1;
		if (checkLock(15) < 1) {
			unlock(15);
			levelUp();
		}
	}
	if (DEBUG == 1) {
		Serial.println(overall_score);
		Serial.println(current_level);
	}
	return;
}

void unlock(int index) {
	if (index > 0 && index < 19) {
		unlocked[index] = byte(255);
	}
	return;
}

int checkLock(int index) {
	if (unlocked[index] < 1) {
		return 0;
	}
	return 1;
}

int getNextLevel() {
	return (current_level + 1) * 144;
}

void clearAnimBoard() {
	for (int i = 0; i < 12; i++) {
		anim_board[i] = N;
	}
}

void setLevel() {
	if (overall_score > 144 && overall_score <= 288) {
		if (checkLock(1) < 1) {
			current_level = 1;
			unlock(1);
			unlock(13);
			levelUp();
			callMod(1, 1, 1, 1);
		}
	}
	if (overall_score > 288 && overall_score <= 432) {
		if (checkLock(2) < 1) {
			current_level = 2;
			unlock(1);
			unlock(2);
			unlock(13);
			unlock(14);
			levelUp();
			callMod(2, 1, 1, 1);
		}
	}
	if (overall_score > 432 && overall_score <= 576) {
		if (checkLock(3) < 1) {
			current_level = 3;
			unlock(1);
			unlock(2);
			unlock(3);
			unlock(14);
			unlock(13);
			levelUp();
			callMod(3, 1, 1, 1);
		}
	}
	if (overall_score > 576 && overall_score <= 720) {
		if (checkLock(4) < 1) {
			current_level = 4;
			unlock(4);
			unlock(3);
			unlock(2);
			unlock(14);
			unlock(1);
			unlock(13);
			levelUp();
			callMod(4, 1, 1, 1);
		}
	}
	if (overall_score > 720 && overall_score <= 864) {
		if (checkLock(5) < 1) {
			current_level = 5;
			unlock(5);
			unlock(4);
			unlock(3);
			unlock(2);
			unlock(14);
			unlock(1);
			unlock(13);
			levelUp();
			callMod(5, 1, 1, 1);
		}
	}
	if (overall_score > 864 && overall_score <= 1008) {
		if (checkLock(6) < 1) {
			current_level = 6;
			unlock(6);
			unlock(5);
			unlock(4);
			unlock(3);
			unlock(2);
			unlock(14);
			unlock(1);
			unlock(13);
			levelUp();
			callMod(6, 1, 1, 1);
		}
	}
	if (overall_score > 1008 && overall_score <= 1152) {
		if (checkLock(7) < 1) {
			current_level = 7;
			unlock(7);
			unlock(6);
			unlock(5);
			unlock(4);
			unlock(3);
			unlock(2);
			unlock(14);
			unlock(1);
			unlock(13);
			levelUp();
			callMod(7, 1, 1, 1);
		}
	}
	if (overall_score > 1152 && overall_score <= 1296) {
		if (checkLock(8) < 1) {
			current_level = 8;
			unlock(8);
			unlock(7);
			unlock(6);
			unlock(5);
			unlock(4);
			unlock(3);
			unlock(2);
			unlock(14);
			unlock(1);
			unlock(13);
			levelUp();
			callMod(8, 1, 1, 1);
		}
	}
	if (overall_score > 1296 && overall_score <= 1440) {
		if (checkLock(9) < 1) {
			current_level = 9;
			unlock(9);
			unlock(7);
			unlock(6);
			unlock(5);
			unlock(4);
			unlock(3);
			unlock(2);
			unlock(14);
			unlock(1);
			unlock(13);
			levelUp();
			callMod(9, 1, 1, 1);
		}
	}
	if (overall_score > 1440 && overall_score <= 1584) {
		if (checkLock(10) < 1) {
			current_level = 10;
			unlock(10);
			unlock(9);
			unlock(7);
			unlock(6);
			unlock(5);
			unlock(4);
			unlock(3);
			unlock(2);
			unlock(14);
			unlock(1);
			unlock(13);
			levelUp();
			callMod(10, 1, 1, 1);
		}
	}
	if (overall_score > 1584 && overall_score <= 1728) {
		if (checkLock(11) < 1) {
			current_level = 11;
			unlock(11);
			unlock(10);
			unlock(9);
			unlock(7);
			unlock(6);
			unlock(5);
			unlock(4);
			unlock(3);
			unlock(2);
			unlock(14);
			unlock(1);
			unlock(13);
			levelUp();
			callMod(11, 1, 1, 1);
		}
	}
	if (overall_score > 1728) {
		if (checkLock(12) < 1) {
			levelUp();
			unlock(12);
			current_level = 12;
			callMod(12, 1, 1, 1);
		}
		for (int i = 1; i < 19; i++) {
			if (checkLock(i) < 1) {
				unlock(i);
			}
		}
	}
	clearAnimBoard();
	return;
}

/*
Bling functions
*/

void callMod(int index, int color1, int color2, int color3) {
  (*modPtrs[index])(color1, color2, color3);
}

int getUnlockedCount() {
	int c = 0;
	for (int i = 0; i < 13; i++) {
		if (unlocked[i] > byte(0)) {
			c++;
		}
	}
	return c;
}

int getUnlockedAnimIndex(int spec) {
	if (spec == 255) {
		return random(0,current_level);
	} else if ((spec > 0 || spec == 0) && spec < current_level + 1) {
		return spec;//avail[spec];
	} else {
		return 0;
	}
}

void playAnimation(int spec) {
	callMod(getUnlockedAnimIndex(spec), random(1,7), random(1,7), random(1,7) );
	charlieplex.clear();
	clearAnimBoard();
	return;
}



/*
score animations
*/

void displayScore_progressiveFill() {
	int score = 144 - (getNextLevel() - overall_score);
	for (int i = 0; i < score; i += 12) {
		for (int d = 0; d < 5; d++) {
			for (int k = 1; k < 13; k++) {
				if (k - 1 < i / 12) {
					blink(B, k, 1);
				} else {
					blink(N, k, 1);
				}
			}
		}
	}
}


void dispaly_test_overall_score_fill (int score) {
	for (int del = 0; del < 10; del++) {
		for (int index = 0; index < 12; index++) {
			if (index < current_level || index == current_level) {
				blink(G, index + 1, 1);
			} else {
				blink(N, index + 1, 1);
			}
		}
	}
	return;
}

void levelUp() {
	int j = 1;
	for (int zzz = 0; zzz < 10; zzz++) {
		for (int kz = 1; kz < 13; kz++) {
                blink(j, kz, 10);
                j++;
                if (j > 7) {
                	j = 1;
                }
        }
        charlieplex.clear();
	}
	displayScore_progressiveFill();
	return;
}

/*
Games
*/

void games() {
	int consecutive_wins = 1;
	int tmp = 0;
	unsigned int timer = 0;
	int btn = 0;
	int reaction_color = W;
	int reaction_speed = 12;
	int index = 1;
	int color = 1;
	int bet = 6;
	int bet_amount = 1;
	bool ready = false;
	int ball = 0;
	bool spinning = true;
	int ticks = 0;
	int speed = 1;
	int mhand = 0;
	int shand = 0;
	int pmhand = 0;
	int pshand = 0;
	int countdown = 0;
	long microcounter = 0;
	unsigned long clockspincounter = 0;
	unsigned long clock_spin_additive = 100000;
	buttonBounce();
	while (true) {
REACTION:
		btn = buttonTime(1000,PB);
		if (btn > 999) {
			goto ENDGAME;
		} else if (btn > 0) {
			charlieplex.clear();
			buttonBounce();
			tmp = 0;
			btn = 0;
			goto ROULETTE;
		}
		buttonBounce();
		btn = 0;
		for (int gb = 0; gb < 12; gb++) {
			game_board[gb] = N;
		}
		for (int i = 0; i < 12; i++) {
			timer++;
			if (consecutive_wins > 14) {
				reaction_color = random(1,7);
			}
			game_board[i] = reaction_color;
			for (int k = 0; k < 12; k++) {
				timer++;
                blink(game_board[k], k + 1, reaction_speed);
                if (digitalRead(PB) == LOW) {
                	goto REACTION;
                }
				if (digitalRead(PA) == LOW) {
					buttonBounce();
					timer = 0;
					if (i == 6) {
						game_board[i] = G;
						for (int zzz = 0; zzz < 10; zzz++) {
							for (int kz = 1; kz < 13; kz++) {
					                blink(game_board[kz - 1], kz, 12);
					        }
					        charlieplex.clear();
						}
						game_board[i] = N;
						consecutive_wins++;
						updateScore(1 * consecutive_wins);
					} else {
						consecutive_wins = 1;
						game_board[i] = R;
						for (int zzz = 0; zzz < 5; zzz++) {
							for (int kx = 1; kx < 13; kx++) {
					                blink(game_board[kx - 1], kx, 25);
					        }
					        charlieplex.clear();
						}
						game_board[i] = N;
					}
				}
			}
			game_board[i] = N;
			if (consecutive_wins < 3) {
				reaction_color = W;
				reaction_speed = 3;
			} else if (consecutive_wins == 3 || consecutive_wins == 4) {
				reaction_color = R;
				reaction_speed = 3;
			} else if (consecutive_wins == 5 || consecutive_wins == 6) {
				reaction_color = B;
				reaction_speed = 3;
			} else if (consecutive_wins == 7 || consecutive_wins == 8) {
				reaction_color = T;
				reaction_speed = 3;
			} else if (consecutive_wins == 9 || consecutive_wins == 10) {
				reaction_color = M;
				reaction_speed = 3;
			} else if (consecutive_wins == 11 || consecutive_wins == 12) {
				reaction_color = Y;
				reaction_speed = 3;
			} else if (consecutive_wins == 13 || consecutive_wins == 14) {
				reaction_color = G;
				reaction_speed = 3;
			} 
		}
		timer++;
		if (timer > 15000) {
			goto ENDGAME;
		}
		goto REACTION;
ROULETTE:
		btn = buttonTime(1000,PB);
		if (btn > 999) {
			goto ENDGAME;
		} else if (btn > 0) {
			charlieplex.clear();
			buttonBounce();
			tmp = 0;
			btn = 0;
			goto CLOCK;
		}
		if (checkLock(13) < 1) {
			goto CLOCK;
		}
		buttonBounce();
		consecutive_wins = 1;
		btn = 0;
		for (int gb = 0; gb < 12; gb += 2) {
			game_board[gb] = R;
			game_board[gb + 1] = N;
		}
		bet = 6;
		bet_amount = 1;
		ready = false;
		while (!ready) {
			for (int l = 0; l < 10; l++) {
				for (int i = 0; i < 13; i++) {
					blink(game_board[i], i + 1, 1);
				}
			}
			tmp = game_board[bet];
			game_board[bet] = W;
			for (int l = 0; l < 10; l++) {
				for (int i = 0; i < 13; i++) {
					blink(game_board[i], i + 1, 1);
				}
			}
			game_board[bet] = tmp;
			tmp = 0;
			if (digitalRead(PA) == LOW) {
				buttonBounce();
				ready = true;
			}
			if (digitalRead(PU) == LOW) {
				bet++;
				if (bet > 11) {
					bet = 0;
				}
			}
			if (digitalRead(PD) == LOW) {
				bet--;
				if (bet < 0) {
					bet = 11;
				}
			}
			if (digitalRead(PB) == LOW) {
				goto ROULETTE;
			}
			timer++;
		}
		ready = false;
		timer = 0;
		while (!ready) {
			for (int l = 0; l < 10; l++) {
				for (int i = 0; i < 13; i++) {
					if (i + 1 <= bet_amount) {
						blink(G, i + 1, 1);
					} else {
						blink(N, i + 1, 1);
					}
				}
			}
			if (digitalRead(PA) == LOW) {
				buttonBounce();
				ready = true;
			}
			if (digitalRead(PU) == LOW) {
				bet_amount++;
				if (bet_amount > 12) {
					bet_amount = 1;
				}
			}
			if (digitalRead(PD) == LOW) {
				bet_amount--;
				if (bet_amount < 1) {
					bet_amount = 12;
				}
			}
			if (digitalRead(PB) == LOW) {
				goto ROULETTE;
			}
			timer++;
		}
		updateScore( 0 - bet_amount );
		timer = 0;
		ready = false;
		ball = 0;
		spinning = true;
		ticks = 0;
		speed = 1;
		for (int gb = 0; gb < 12; gb += 2) {
			game_board[gb] = R;
			game_board[gb + 1] = N;
		}
		while (spinning) {
			ticks = random(128,256);
			while (ticks > 0) {
				for (int b = 0; b < 12; b++) {
					for (int m = 0; m < speed; m++) {
						tmp = game_board[b];
						game_board[b] = W;
						for (int n = 0; n < 12; n++) {
							blink(game_board[n], n + 1, 1);
						}
						game_board[b] = tmp;
						ball = b;
						ticks--;
						if (ticks <= 0) {
							break;
						}
					}
					if (ticks <= 0) {
						break;
					}
				}
				speed++;
			}
			spinning = false;
		}
		if (ball == bet) {
			game_board[ball] = G;
			for (int zzz = 0; zzz < 25; zzz++) {
				for (int kz = 1; kz < 13; kz++) {
		                blink(game_board[kz - 1], kz, 3);
		        }
		        charlieplex.clear();
			}
			game_board[ball] = N;
			consecutive_wins++;
			updateScore((bet_amount + consecutive_wins) * 12);
			timer = 0;
		} else if ((ball % 2 && bet % 2) || (!(ball % 2) && !(bet % 2))) {
			game_board[ball] = B;
			for (int zzz = 0; zzz < 25; zzz++) {
				for (int kz = 1; kz < 13; kz++) {
		                blink(game_board[kz - 1], kz, 3);
		        }
		        charlieplex.clear();
			}
			game_board[ball] = N;
			updateScore((bet_amount + consecutive_wins) * 2);
			timer = 0;
		} else {
			game_board[ball] = R;
			for (int zzz = 0; zzz < 25; zzz++) {
				for (int kz = 1; kz < 13; kz++) {
		                blink(game_board[kz - 1], kz, 3);
		        }
		        charlieplex.clear();
			}
			game_board[ball] = N;
			consecutive_wins = 1;
			timer = 0;
		}
		for (int gb = 0; gb < 12; gb += 2) {
			game_board[gb] = R;
			game_board[gb + 1] = N;
		}
		timer++;
		if (timer > 15000) {
			goto ENDGAME;
		}
		goto ROULETTE;
CLOCK:
		btn = buttonTime(1000,PB);
		if (btn > 999) {
			goto ENDGAME;
		} else if (btn > 0) {
			charlieplex.clear();
			buttonBounce();
			tmp = 0;
			btn = 0;
			goto LEDTEST;
		}
		if (checkLock(14) < 1) {
			goto LEDTEST;
		}
		buttonBounce();
		consecutive_wins = 1;
		mhand = random(0,11);
		shand = random(0,11);
		if (mhand == shand) {
			if (shand == 11) {
				shand = 0;
			} else {
				shand++;
			}
		}
		pmhand = random(0,11);
		pshand = random(0,11);
		ready = false;
		while (!ready) {
			for (int i = 0; i < 12; i++) {
				if (i == mhand) {
					blinkb(B, i + 1, 1, brightness);
				} else if (i == shand) {
					blinkb(G, i + 1, 1, brightness);
				} else {
					blinkb(N, i + 1, 1, brightness);
				}
			}
			if (digitalRead(PA) == LOW) {
				buttonBounce();
				ready = true;
			}
			if (digitalRead(PB) == LOW) {
				goto CLOCK;
			}
			timer++;
		}
		timer = 0;
		ready = false;
		spinning = true;
		ticks = 0;
		speed = 2;
		while (spinning) {
			ticks = random(75,100);
			while (ticks > 0) {
				while (pmhand < 12) {
					while (pshand < 12) {
						for (int m = 0; m < speed; m++) {
							for (int i = 0; i < 12; i++) {
								if (i == mhand) {
									blink(B, i + 1, 1);
								} else if (i == shand) {
									blink(G, i + 1, 1);
								} else if (i == pmhand) {
									blinkb(B, i + 1, 1, brightness);
								} else if (i == pshand) {
									blinkb(G, i + 1, 1, brightness);
								} else {
									blinkb(N, i + 1, 1, brightness);
								}
							}
							ticks--;
							if (ticks <= 0) {
								break;
							}
						}
						if (ticks <= 0) {
							break;
						}
						pshand++;
					}
					if (ticks <= 0) {
						break;
					}
					speed++;
					pmhand++;
					pshand = 0;
				}
				if (ticks <= 0) {
					break;
				}
				pmhand = 0;
				pshand = 0;
			}
			spinning = false;
		}
		countdown = 1000;
		ready = false;
		microcounter = micros();
		clockspincounter = microcounter;
		while (countdown > 0 && !ready) {
			for (int i = 0; i < 12; i++) {
				if (i == pmhand && (pmhand == mhand)) {
					blinkb(W, i + 1, 1, brightness);
				} else if (i == pshand && (pshand == shand)) {
					blinkb(W, i + 1, 1, brightness);
				} else if (i == pmhand && (pmhand == shand)) {
					blinkb(R, i + 1, 1, brightness);
				} else if (i == pshand && (pshand == mhand)) {
					blinkb(R, i + 1, 1, brightness);
				} else if (i == pmhand) {
					if (countdown % 4 == 0) {
						blinkb(B, i + 1, 1, 975);
					} else {
						blinkb(N, i + 1, 1, 300);
					}
				} else if (i == pshand) {
					if (countdown % 4 == 0) {
						blinkb(G, i + 1, 1, 975);
					} else {
						blinkb(N, i + 1, 1, 300);
					}
				} else if (i == mhand) {
					blinkb(B, i + 1, 1, brightness);
				} else if (i == shand) {
					blinkb(G, i + 1, 1, brightness);
				} else {
					blinkb(N, i + 1, 1, brightness);
				}
			}
			if (digitalRead(PA) == LOW) {
				buttonBounce();
				timer = 0;
				ready = true;
			}
			if (digitalRead(PB) == LOW) {
				delay(1);
			}
			if (digitalRead(PU) == LOW) {
				timer = 0;
				if (micros() > clockspincounter + clock_spin_additive){
					pshand++;
					clock_spin_additive -= 5000;
					if (pshand > 11) {
						pshand = 0;
						pmhand++;
						if (pmhand > 11) {
							pmhand = 0;
						}
					}
					clockspincounter = micros();
				}
			} else if (digitalRead(PD) == LOW) {
				timer = 0;
				if (micros() > clockspincounter + clock_spin_additive){
					pshand--;
					clock_spin_additive -= 5000;
					if (pshand < 0) {
						pshand = 11;
						pmhand--;
						if (pmhand < 0) {
							pmhand = 11;
						}
					}
					clockspincounter = micros();
				}
			} else {
				clock_spin_additive = 100000;
			}
			countdown--;
		}
		if (ready && countdown > 0 && pmhand == mhand && pshand == shand) {
			for (int dd = 0; dd < 10; dd++) {
				for (int kz = 1; kz < 13; kz++) {
		                blink(G, kz, 3);
		        }
		    }
	        charlieplex.clear();
	        if (countdown > 900) {
	        	updateScore(5 * consecutive_wins);
	        } else if (countdown > 700) {
	        	updateScore(4 * consecutive_wins);
	        } else if (countdown > 500) {
	        	updateScore(3 * consecutive_wins);
	        } else if (countdown > 300) {
	        	updateScore(2 * consecutive_wins);
	        } else if (countdown > 0) {
	        	updateScore(1 * consecutive_wins);
	        }
	        consecutive_wins++;
		} else {
			for (int dd = 0; dd < 10; dd++) {		
				for (int kz = 1; kz < 13; kz++) {
		                blink(R, kz, 3);
		        }
		    }
	        charlieplex.clear();
	        consecutive_wins = 1;
		}
		timer++;
		if (timer > 15000) {
			goto ENDGAME;
		}
		goto CLOCK;
LEDTEST:
		if (checkLock(15) < 1) {
			goto REACTION;
		}
		btn = buttonTime(1000,PB);
		if (btn > 999) {
			goto ENDGAME;
		} else if (btn > 0) {
			charlieplex.clear();
			buttonBounce();
			consecutive_wins = 0;
			tmp = 0;
			btn = 0;
			goto REACTION;
		}
		buttonBounce();
		while (true) {

			blink(color, index, 1);

			if (digitalRead(PA) == LOW) {
				buttonBounce();
				color++;
				if (color > 7) {
					color = 1;
				}

			}
			if (digitalRead(PU) == LOW) {
				buttonBounce();
				index++;
				if (index > 12) {
					index = 1;
				}
			}
			if (digitalRead(PD) == LOW) {
				buttonBounce();
				index--;
				if (index < 1) {
					index = 12;
				}
			}
			if (digitalRead(PB) == LOW) {
				goto LEDTEST;
			}
			timer++;
		}
		timer++;
		if (timer > 15000) {
			goto ENDGAME;
		}
		goto LEDTEST;
	}
ENDGAME:
		updateScore(0);
		saveScore();
		charlieplex.clear();
		buttonBounce();
		return;
}

/*
animations
*/

void chaser(int color1, int color2, int color3) {
	int color = random(1,7);
	for (int l = 0; l < 8; l++) {
		for (int i = 0; i < 12; i++) {
			anim_board[i] = color;
			for (int k = 0; k < 12; k++) {
	            blink(anim_board[k], k + 1, 3);
	            if (checkButtons() > 0) {
	            	return;
	            }
	        }
	        anim_board[i] = N;
	    }
	}
	return;
}

void opchaser(int color1, int color2, int color3) {
	for (int loops = 0; loops < 8; loops++) {
		for (int lindex = 1; lindex < 13; lindex++) {
			for (int fd = 0; fd < 10; fd++) {
				for (int lit = 1; lit < 13; lit++) {
					if (lindex == lit) {
						blink(color1, lit, 3);
						if (checkButtons() > 0) {
			            	return;
			            }
			            blink(color2, 12 - lit, 3);
						if (checkButtons() > 0) {
			            	return;
			            }
					}
				}
			}
		}
	}
}

void police(int color1, int color2, int color3) {
	anim_board[0] = W;
	anim_board[6] = W;
	for (int times = 0; times < 10; times++) {
		for (int j = 1; j < 6; j++) {
			anim_board[j] = R;
		}
		for (int j = 7; j < 12; j++) {
			anim_board[j] = N;
		}
		for (int frame_delay = 0; frame_delay < 10; frame_delay++) {
			for (int k = 0; k < 12; k++) {
	            blink(anim_board[k], k + 1, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
	        }
		}
		for (int j = 1; j < 6; j++) {
			anim_board[j] = N;
		}
		for (int j = 7; j < 12; j++) {
			anim_board[j] = B;
		}
		for (int frame_delay = 0; frame_delay < 10; frame_delay++) {
			for (int k = 0; k < 12; k++) {
	            blink(anim_board[k], k + 1, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
	        }
		}

	}
	return;
}

void poles(int color1, int color2, int color3) {
	for (int loops = 0; loops < 9; loops++) {
		for (int p = 0; p < 6; p+=3) {
			anim_board[p] = color1;
			anim_board[p+1] = N;
			anim_board[p+2] = N;
		}
		for (int p = 11; p > 6; p-=3) {
			anim_board[p] = color3;
			anim_board[p-1] = N;
			anim_board[p-2] = N;
		}
		for (int rep = 0; rep < 10; rep++) {
			for (int k = 0; k < 12; k++) {
	            blink(anim_board[k], k + 1, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
	        }
	    }
		for (int p = 0; p < 6; p+=3) {
			anim_board[p] = N;
			anim_board[p+1] = color1;
			anim_board[p+2] = N;
		}
		for (int p = 11; p > 6; p-=3) {
			anim_board[p] = N;
			anim_board[p-1] = color3;
			anim_board[p-2] = N;
		}
		for (int rep = 0; rep < 10; rep++) {
			for (int k = 0; k < 12; k++) {
	            blink(anim_board[k], k + 1, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
	        }
	    }
		for (int p = 0; p < 6; p+=3) {
			anim_board[p] = N;
			anim_board[p+1] = N;
			anim_board[p+2] = color1;
		}
		for (int p = 11; p > 6; p-=3) {
			anim_board[p] = N;
			anim_board[p-1] = N;
			anim_board[p-2] = color3;
		}
		for (int rep = 0; rep < 10; rep++) {
			for (int k = 0; k < 12; k++) {
	            blink(anim_board[k], k + 1, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
	        }
	    }

	}
	return;
}

void rotate(int color1, int color2, int color3) {
	if (anim_board[0] == N && anim_board[1] == N && anim_board[2] == N && anim_board[3] == N && 
		anim_board[4] == N && anim_board[5] == N && anim_board[6] == N && anim_board[7] == N && 
		anim_board[8] == N && anim_board[9] == N && anim_board[10] == N && anim_board[11] == N)
	{
		for (int i = 0; i < 12; i+=3) {
			anim_board[i] = color1;
			anim_board[i+1] = color2;
			anim_board[i+2] = N;
		}
	}
	for (int loops = 0; loops < 48; loops++) {
		for (int repeat = 0; repeat < 10; repeat++) {
			for (int k = 0; k < 12; k++) {
	            blink(anim_board[k], k + 1, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
	        }
	    }
        int tmp = anim_board[11];
        for (int i = 11; i > 0; i--) {
        	anim_board[i] = anim_board[i - 1];
        }
        anim_board[0] = tmp;
	}
	return;
}

void rainbow(int color1, int color2, int color3) {
	int j = 1;
	for (int i = 0; i < 12; i+=2) {
		anim_board[i] = j;
		anim_board[i+1] = j;
		j++;
	}
	rotate(0,0,0);
	return;
}

void fakepwmchaser(int color1, int color2, int color3) {
	int bness;
	for (int l = 0; l < 8; l++) {
		for (int i = 0; i < 12; i++) {
			bness = 250;
			for (int k = 0; k < 12; k++) {
				if (k == i) {
					blinkb(color1, k + 1, 5, bness);
				} else {
					blinkb(N, k + 1, 5, bness);
				}
	            bness += 50;
	            if (checkButtons() > 0) {
	            	return;
	            }
	        }
	    }
	}
	return;
}

void fakepwmpulse(int color1, int color2, int color3) {
	int dir = 0;
	int stop = 0;
	int b = 0;
	int flash = color1;
	for (int a = 0; a < 12; a++) {
		while (stop < 1) {
			for (int i = 0; i < 12; i++) {
				blinkb(flash, i + 1, 1, b);
			}
			if (checkButtons() > 0) {
            	return;
            }
			if (b > 900) {
				dir = 1;
			}
			if (dir < 1 && b < 900) {
				b += 30;
			} else if (dir > 0 && b > 0) {
				b -= 30;
			} else {
				stop++;
			}
		}
		stop = 0;
		b = 0;
	}
	return;
}

void pacman(int color1, int color2, int color3) {
	for (int doubler = 0; doubler < 2; doubler++) {
		for (int loop = 0; loop < 3; loop++) {
			for (int i = 0; i < 10; i++) {
				for (int k = 1; k < 13; k++) {
					blink(Y, k, 1);
		            if (checkButtons() > 0) {
		            	return;
		            }
				}
			}
			for (int i = 0; i < 10; i++) {
				for (int k = 1; k < 13; k++) {
					if (k != 10) {
						blink(Y, k, 1);
			            if (checkButtons() > 0) {
			            	return;
			            }
					}
				}
			}
			for (int i = 0; i < 10; i++) {
				for (int k = 1; k < 13; k++) {
					if (k != 10 && k != 9 && k != 11) {
						blink(Y, k, 1);
			            if (checkButtons() > 0) {
			            	return;
			            }
					}
				}
			}
		}
		for (int i = 0; i < 10; i++) {
			for (int k = 1; k < 13; k++) {
				blink(Y, k, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int k = 1; k < 13; k++) {
			if (k != 10) {
				blink(Y, k, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int k = 1; k < 13; k++) {
			if (k != 10 && k != 9 && k != 11) {
				blink(Y, k, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int k = 1; k < 13; k++) {
			if (k != 10 && k != 9 && k != 11 && k != 12 && k != 8) {
				blink(Y, k, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int k = 1; k < 13; k++) {
			if (k != 10 && k != 9 && k != 11 && k != 12 && k != 8 && k != 7 && k != 1) {
				blink(Y, k, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int k = 1; k < 13; k++) {
			if (k == 2 || k == 3 || k == 4 || k == 5 || k == 6) {
				blink(Y, k, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int k = 1; k < 13; k++) {
			if (k == 3 || k == 4 || k == 5) {
				blink(Y, k, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
			}
		}
	}
	for (int i = 0; i < 10; i++) {
		for (int k = 1; k < 13; k++) {
			if (k == 4) {
				blink(Y, k, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
			}
		}
	}

	for (int i = 0; i < 10; i++) {
		for (int k = 1; k < 13; k++) {
			if (k % 2 != 0) {
				blink(Y, k, 1);
	            if (checkButtons() > 0) {
	            	return;
	            }
			}
		}
	}
	return;
}

void dualchaser(int color1, int color2, int color3) {
	for (int loops = 0; loops < 8; loops++) {
		for (int lindex = 1; lindex < 13; lindex++) {
			for (int fd = 0; fd < 10; fd++) {
				for (int lit = 1; lit < 13; lit++) {
					if (lindex == lit) {
						blink(color1, lit, 3);
						if (checkButtons() > 0) {
			            	return;
			            }					
			            if (lit < 7) {
							blink(color2, lit + 6, 3);
							if (checkButtons() > 0) {
			            		return;
			            	}
						} else {
							blink(color2, lit - 6, 3);
							if (checkButtons() > 0) {
			            		return;
			            	}
						}
					}
				}
			}
		}
	}
}

void fill(int color1, int color2, int color3) {
	for (int i = 1; i < 8; i++) {
		for (int j = 0; j < 12; j++) {
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 12; l++) {
					if (l <= j) {
						blink(i, l + 1, 1);
			            if (checkButtons() > 0) {
			            	return;
			            }
					} else {
						if (i - 1 > -1) {
							blink(i - 1, l + 1, 1);
						} else {
							blink(N, l + 1, 1);
						}
					}
				}
			}
		}
	}
	return;
}

void cylon(int color1, int color2, int color3) {
	for (int l = 0; l < 8; l++) {
		for (int i = 4; i < 12; i++) {
			for (int d = 0; d < 5; d++) {
				for (int j = 0; j < 12; j++) {
					if ((j <= i) && (j >= i - 4)) {
						blink(color1, j + 1, 3);
			            if (checkButtons() > 0) {
			            	return;
			            }
					}
				}
			}
		}
		for (int i = 12; i > 0; i--) {
			for (int d = 0; d < 5; d++) {
				for (int j = 12; j > -1; j--) {
					if ((j >= i) && (j <= i + 4)) {
						blink(color1, j + 1, 3);
		    	        if (checkButtons() > 0) {
		    	        	return;
		    	        }
					}
				}
			}
		}

	}
	return;
}

void fakepwmslow(int color1, int color2, int color3) {
	for (int i = 0; i < 12; i+=3) {
		anim_board[i] = color1;
		anim_board[i+1] = color2;
		anim_board[i+2] = color3;
	}

	long previousMicros = 0;
	long intervalLength = 400;
	long onInterval = 100;
	long currentInterval;
	int  fadeLength = 300;
	int  fadeCount = 0;
	int  intervalDirection = 1;
	long loops = 0;
	for (int k = 0; k < 12; k++) {
		while (loops < 2) {
            if (checkButtons() > 0) {
            	return;
            }
			unsigned long currentMicros = micros();
			currentInterval = currentMicros - previousMicros;
			if (currentInterval <= onInterval) {
		        	_blink(anim_board[k], k + 1, 0);
			}
			else {
				charlieplex.clear();
			}
			if(currentInterval > intervalLength) {
		    	previousMicros = currentMicros;
		    	fadeCount++;
		    	if (fadeCount > fadeLength) {
					fadeCount = 0;
					onInterval += (intervalDirection * 75);
					if (onInterval <= 75 || onInterval >= intervalLength) {
						intervalDirection *= -1;
						loops++;
					}
				}
			}
		}
		loops = 0;
	}
	return;
}

/*
diag, etc animations
*/

void startup_led_test () {
	for (int j = 1; j < 8; j++) {
		for (int l = 0; l < 10; l++) {
			for (int k = 1; k < 13; k++) {
				blink(j, k, 5);
			}
		}
	}
	return;
}

/*
Based LED functions
*/

void blinkb(char color, int index, int single, int dbrightness) {
	if (dbrightness < 975) {
		unsigned long end = micros() + 1000;
		for (int i = 0; i < single; i++) {
			while (micros() < end) {
				if (micros() <= end - dbrightness) {
					_blink(color, index, 0);
				} else {
					charlieplex.clear();
				}
			}
			end = micros() + 1000;
		}
	} else {
		_blink(color, index, single);
	}
}

void blink(char color, int index, int single) {
	if (brightness < 975) {
		unsigned long end = micros() + 1000;
		for (int i = 0; i < single; i++) {
			while (micros() < end) {
				if (micros() <= end - brightness) {
					_blink(color, index, 0);
				} else {
					charlieplex.clear();
				}
			}
			end = micros() + 1000;
		}
	} else {
		_blink(color, index, single);
	}
}

/*
Base LED functions
*/

void _blink (char color, int index, int single) {
	if (color == R && index == 1) {
		charlieplex.charlieWrite(led1,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 1) {
		charlieplex.charlieWrite(led1,HIGH);
		charlieplex.charlieWrite(led2,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 1) {
		charlieplex.charlieWrite(led2,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 1) {
		charlieplex.charlieWrite(led2,HIGH);
		charlieplex.charlieWrite(led3,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 1) {
		charlieplex.charlieWrite(led3,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 1) {
		charlieplex.charlieWrite(led3,HIGH);
		charlieplex.charlieWrite(led1,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 1) {
		charlieplex.charlieWrite(led2,HIGH);
		charlieplex.charlieWrite(led3,HIGH);
		charlieplex.charlieWrite(led1,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 1) {
		charlieplex.charlieWrite(led2,LOW);
		charlieplex.charlieWrite(led3,LOW);
		charlieplex.charlieWrite(led1,LOW);
		delay(single);
		charlieplex.clear();
	} 

	if (color == B && index == 2) {
		charlieplex.charlieWrite(led6,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 2) {
		charlieplex.charlieWrite(led4,HIGH);
		charlieplex.charlieWrite(led6,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 2) {
		charlieplex.charlieWrite(led4,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 2) {
		charlieplex.charlieWrite(led4,HIGH);
		charlieplex.charlieWrite(led5,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 2) {
		charlieplex.charlieWrite(led5,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 2) {
		charlieplex.charlieWrite(led5,HIGH);
		charlieplex.charlieWrite(led6,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 2) {	  
		charlieplex.charlieWrite(led4,HIGH);
		charlieplex.charlieWrite(led5,HIGH);
		charlieplex.charlieWrite(led6,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 2) {
		charlieplex.charlieWrite(led4,LOW);
		charlieplex.charlieWrite(led5,LOW);
		charlieplex.charlieWrite(led6,LOW);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 3) {
		charlieplex.charlieWrite(led9,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 3) {
		charlieplex.charlieWrite(led7,HIGH);
		charlieplex.charlieWrite(led8,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 3) {
		charlieplex.charlieWrite(led7,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 3) {
		charlieplex.charlieWrite(led7,HIGH);
		charlieplex.charlieWrite(led9,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 3) {
		charlieplex.charlieWrite(led8,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 3) {
		charlieplex.charlieWrite(led8,HIGH);
		charlieplex.charlieWrite(led9,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 3) {
		charlieplex.charlieWrite(led7,HIGH);
		charlieplex.charlieWrite(led8,HIGH);
		charlieplex.charlieWrite(led9,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 3) {
		charlieplex.charlieWrite(led7,LOW);
		charlieplex.charlieWrite(led8,LOW);
		charlieplex.charlieWrite(led9,LOW);
		delay(single);
		charlieplex.clear();
	}


	if (color == R && index == 4) {
		charlieplex.charlieWrite(led10,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 4) {
		charlieplex.charlieWrite(led10,HIGH);
		charlieplex.charlieWrite(led11,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 4) {
		charlieplex.charlieWrite(led11,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 4) {
		charlieplex.charlieWrite(led11,HIGH);
		charlieplex.charlieWrite(led12,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 4) {
		charlieplex.charlieWrite(led12,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 4) {
		charlieplex.charlieWrite(led10,HIGH);
		charlieplex.charlieWrite(led12,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 4) {
		charlieplex.charlieWrite(led10,HIGH);
		charlieplex.charlieWrite(led11,HIGH);
		charlieplex.charlieWrite(led12,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 4) {
		charlieplex.charlieWrite(led10,LOW);
		charlieplex.charlieWrite(led11,LOW);
		charlieplex.charlieWrite(led12,LOW);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 5) {
		charlieplex.charlieWrite(led15,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 5) {
		charlieplex.charlieWrite(led13,HIGH);
		charlieplex.charlieWrite(led15,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 5) {
		charlieplex.charlieWrite(led13,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 5) {
		charlieplex.charlieWrite(led13,HIGH);
		charlieplex.charlieWrite(led14,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 5) {
		charlieplex.charlieWrite(led14,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 5) {
		charlieplex.charlieWrite(led14,HIGH);
		charlieplex.charlieWrite(led15,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 5) {
		charlieplex.charlieWrite(led14,HIGH);
		charlieplex.charlieWrite(led15,HIGH);
		charlieplex.charlieWrite(led13,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 5) {
		charlieplex.charlieWrite(led14,LOW);
		charlieplex.charlieWrite(led15,LOW);
		charlieplex.charlieWrite(led13,LOW);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 6) {
		charlieplex.charlieWrite(led18,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 6) {
		charlieplex.charlieWrite(led17,HIGH);
		charlieplex.charlieWrite(led18,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 6) {
		charlieplex.charlieWrite(led17,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 6) {
		charlieplex.charlieWrite(led16,HIGH);
		charlieplex.charlieWrite(led17,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 6) {
		charlieplex.charlieWrite(led16,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 6) {
		charlieplex.charlieWrite(led16,HIGH);
		charlieplex.charlieWrite(led18,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 6) {
		charlieplex.charlieWrite(led16,HIGH);
		charlieplex.charlieWrite(led17,HIGH);
		charlieplex.charlieWrite(led18,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 6) {
		charlieplex.charlieWrite(led16,LOW);
		charlieplex.charlieWrite(led17,LOW);
		charlieplex.charlieWrite(led18,LOW);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 7) {
		charlieplex.charlieWrite(led21,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 7) {
		charlieplex.charlieWrite(led20,HIGH);
		charlieplex.charlieWrite(led21,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 7) {
		charlieplex.charlieWrite(led20,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 7) {
		charlieplex.charlieWrite(led19,HIGH);
		charlieplex.charlieWrite(led20,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 7) {
		charlieplex.charlieWrite(led19,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 7) {
		charlieplex.charlieWrite(led19,HIGH);
		charlieplex.charlieWrite(led21,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 7) {
		charlieplex.charlieWrite(led19,HIGH);
		charlieplex.charlieWrite(led20,HIGH);
		charlieplex.charlieWrite(led21,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 7) {
		charlieplex.charlieWrite(led19,LOW);
		charlieplex.charlieWrite(led20,LOW);
		charlieplex.charlieWrite(led21,LOW);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 8) {
		charlieplex.charlieWrite(led23,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 8) {
		charlieplex.charlieWrite(led22,HIGH);
		charlieplex.charlieWrite(led23,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 8) {
		charlieplex.clear();
		charlieplex.charlieWrite(led22,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 8) {
		charlieplex.charlieWrite(led22,HIGH);
		charlieplex.charlieWrite(led24,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 8) {
		charlieplex.charlieWrite(led24,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 8) {
		charlieplex.charlieWrite(led23,HIGH);
		charlieplex.charlieWrite(led24,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 8) {
		charlieplex.charlieWrite(led22,HIGH);
		charlieplex.charlieWrite(led23,HIGH);
		charlieplex.charlieWrite(led24,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 8) {
		charlieplex.charlieWrite(led22,LOW);
		charlieplex.charlieWrite(led23,LOW);
		charlieplex.charlieWrite(led24,LOW);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 10) {
		charlieplex.charlieWrite(led25,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 10) {
		charlieplex.charlieWrite(led25,HIGH);
		charlieplex.charlieWrite(led26,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 10) {
		charlieplex.charlieWrite(led26,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 10) {
		charlieplex.charlieWrite(led26,HIGH);
		charlieplex.charlieWrite(led27,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 10) {
		charlieplex.charlieWrite(led27,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 10) {
		charlieplex.charlieWrite(led25,HIGH);
		charlieplex.charlieWrite(led27,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 10) {
		charlieplex.charlieWrite(led26,HIGH);
		charlieplex.charlieWrite(led27,HIGH);
		charlieplex.charlieWrite(led25,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 10) {
		charlieplex.charlieWrite(led26,LOW);
		charlieplex.charlieWrite(led27,LOW);
		charlieplex.charlieWrite(led25,LOW);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 9) {
		charlieplex.charlieWrite(led29,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 9) {
		charlieplex.charlieWrite(led28,HIGH);
		charlieplex.charlieWrite(led29,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 9) {
		charlieplex.charlieWrite(led28,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 9) {
		charlieplex.charlieWrite(led28,HIGH);
		charlieplex.charlieWrite(led30,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 9) {
		charlieplex.charlieWrite(led30,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 9) {
		charlieplex.charlieWrite(led29,HIGH);
		charlieplex.charlieWrite(led30,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 9) {
		charlieplex.charlieWrite(led28,HIGH);
		charlieplex.charlieWrite(led29,HIGH);
		charlieplex.charlieWrite(led30,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 9) {
		charlieplex.charlieWrite(led28,LOW);
		charlieplex.charlieWrite(led29,LOW);
		charlieplex.charlieWrite(led30,LOW);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 12) {
		charlieplex.charlieWrite(led32,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 12) {
		charlieplex.charlieWrite(led32,HIGH);
		charlieplex.charlieWrite(led33,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 12) {
		charlieplex.charlieWrite(led33,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 12) {
		charlieplex.charlieWrite(led31,HIGH);
		charlieplex.charlieWrite(led33,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 12) {
		charlieplex.charlieWrite(led31,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 12) {
		charlieplex.charlieWrite(led31,HIGH);
		charlieplex.charlieWrite(led32,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 12) {
		charlieplex.charlieWrite(led31,HIGH);
		charlieplex.charlieWrite(led32,HIGH);
		charlieplex.charlieWrite(led33,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 12) {
		charlieplex.charlieWrite(led31,LOW);
		charlieplex.charlieWrite(led32,LOW);
		charlieplex.charlieWrite(led33,LOW);
		delay(single);
		charlieplex.clear();
	}

	if (color == G && index == 11) {
		charlieplex.charlieWrite(led34,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == Y && index == 11) {
		charlieplex.charlieWrite(led34,HIGH);
		charlieplex.charlieWrite(led35,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == R && index == 11) {
		charlieplex.charlieWrite(led35,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == M && index == 11) {
		charlieplex.charlieWrite(led35,HIGH);
		charlieplex.charlieWrite(led36,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == B && index == 11) {
		charlieplex.charlieWrite(led36,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == T && index == 11) {
		charlieplex.charlieWrite(led34,HIGH);
		charlieplex.charlieWrite(led36,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == W && index == 11) {
		charlieplex.charlieWrite(led34,HIGH);
		charlieplex.charlieWrite(led35,HIGH);
		charlieplex.charlieWrite(led36,HIGH);
		delay(single);
		charlieplex.clear();
	}

	if (color == N && index == 11) {
		charlieplex.charlieWrite(led34,LOW);
		charlieplex.charlieWrite(led35,LOW);
		charlieplex.charlieWrite(led36,LOW);
		delay(single);
		charlieplex.clear();
	}
}
