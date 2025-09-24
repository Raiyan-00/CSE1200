#include "iGraphics.h"
#include <time.h>
#include <string.h> 
#include <windows.h> 
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              CONSTANTS
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;
const int JUMP_STRENGTH = 30;
const int GRAVITY = 2;
const int WALK_SPEED = 3;
const int RUN_SPEED = 10;
const int GROUND_Y = 36;
const int CHARACTER_WIDTH = 96;
const int CHARACTER_HEIGHT = 96;
const int LEVEL_4_ENEMY_WIDTH = 292; // Twice the character width
const int LEVEL_4_ENEMY_HEIGHT = 292; // Twice the character height
const int ENEMY_WIDTH = 96;
const int ENEMY_HEIGHT = 96;
const int ENEMY_SPEED = 4;
const int MAX_ENEMIES = 12;
const int HITS_TO_STUN = 1;
const int HITS_TO_KILL = 2;
const int ATTACK_RANGE = 15;
const int CHARACTER_MAX_HEALTH = 100;
const int ENEMY_MAX_HEALTH = 100;
const int COLLISION_DAMAGE = 10;
const int ENEMIES_PER_LEVEL = 10;
const int ENEMIES_PER_WAVE = 2;
const int SKELETON_ATTACK_FRAMES = 12;
const int TROLL_ATTACK_FRAMES = 10;
const int ENEMY_ATTACK_COOLDOWN = 180;
const int ENEMY_ATTACK_RANGE = 70;
const int ENEMY_ATTACK_DAMAGE = 8;
const int MIN_DISTANCE_TO_CHARACTER = (CHARACTER_WIDTH / 2 + ENEMY_WIDTH / 2 + 5) / 2;
const int PREDATOR_ATTACK_FRAMES = 4;
const int PREDATOR_WALK_FRAMES = 9;
const int PREDATOR_DEATH_FRAMES = 5;
const int DEMON_WALK_FRAMES = 12; // Added for Level 4 Demon Boss
const int DEMON_ATTACK_FRAMES = 15; // Added for Level 4 Demon Boss Attack
const int DEMON_DEATH_FRAMES = 22; // Added for Level 4 Demon Boss Death
const int LEVEL_4_HITS_TO_KILL = 15; // Hits needed to kill Level 4 boss
const int LEVEL_4_CHARACTER_DEATH_HITS = 4;
const int LEVEL_4_STUN_DURATION = 5; // 30 frames = 0.6 seconds at 50fps (20ms timer)
// Hits from boss that kill character
// --------------------------------------------------------------------------------------
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              ENUMS
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
enum CharacterState {
	STATE_IDLE,
	STATE_WALKING,
	STATE_RUNNING,
	STATE_JUMPING,
	STATE_ATTACKING,
	STATE_DYING,
	STATE_DEATH_ANIM // New state for death animation sequence
};
enum Direction {
	DIRECTION_RIGHT = 0,
	DIRECTION_LEFT = 1
};
enum EnemyState {
	ENEMY_WALKING,
	ENEMY_STUNNED,
	ENEMY_DYING,
	ENEMY_ATTACKING // New state for enemy attacking
};
enum EnemyDirection {
	ENEMY_DIRECTION_LEFT = 0,
	ENEMY_DIRECTION_RIGHT = 1
};
enum EnemyType {
	ENEMY_TYPE_SKELETON = 0,
	ENEMY_TYPE_TROLL = 1,
	ENEMY_TYPE_PREDATOR = 2, // New enemy type
	ENEMY_TYPE_DEMON = 3     // Added for Level 4 Final Boss
};
enum GameLevel {
	LEVEL_1 = 1,
	LEVEL_2 = 2,
	LEVEL_3 = 3,
	LEVEL_4 = 4 // Added Level 4
};
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              STRUCTS
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
struct GameState {
	bool showMenu;
	bool showInstructions;
	bool showAbout;
	bool showHighScores;
	bool showLevelSelect; // <<< ADDED FOR LEVEL JUMPING
	bool gameStarted;
	bool enemiesSpawned;
	int enemiesKilled;
	int nextEnemyToSpawn;
	bool characterDead;
	int characterHealth;
	int currentLevel;
	int enemiesSpawnedInLevel;
	bool levelComplete;
	bool awaitingLevelProgression;
	bool deathAnimationFinished;
	bool isPaused;
	int currentScore;
	int damageTaken;
};
struct Images {
	// Menu images
	int titleImage;
	int playButtonImage, instructionsButtonImage, aboutButtonImage, exitButtonImage, clickImage;
	int highScoresButtonImage;
	int levelButtonImage; // <<< ADDED FOR LEVEL JUMPING
	int backGround;
	int gameBackGround;
	int returnButtonImage;
	int pauseButtonImage;
	int resumeButtonImage;
	int idleSprite[2];
	int walkSprites[2][8];
	int runSprites[2][8];
	int jumpSprites[2][12];
	int attackSprites[2][5];
	int dyingSprite[2];
	int deathSprites[2][4];
	// Enemy sprites - Level 1 (Skeletons)
	int enemyWalkSprites_L1[24];
	int enemyWalkSpritesRight_L1[24];
	int enemyStunnedSprite_L1;
	int enemyDyingSprites_L1[15];
	// Enemy attack sprites - Level 1 (Skeletons)
	int enemyAttackSprites_L1[SKELETON_ATTACK_FRAMES];
	int enemyAttackSpritesRight_L1[SKELETON_ATTACK_FRAMES];
	// Enemy sprites - Level 2 (Trolls)
	int enemyWalkSprites_L2[10];
	int enemyWalkSpritesRight_L2[10];
	int enemyStunnedSprite_L2_Left;
	int enemyStunnedSprite_L2_Right;
	int enemyDyingSprites_L2_Left[10];
	int enemyDyingSprites_L2_Right[10];
	// Enemy attack sprites - Level 2 (Trolls)
	int enemyAttackSprites_L2[TROLL_ATTACK_FRAMES];
	int enemyAttackSpritesRight_L2[TROLL_ATTACK_FRAMES];
	// ----------------------------------
	// Enemy sprites - Level 3 (Predators)
	int enemyIdleSprite_L3_Left;
	int enemyIdleSprite_L3_Right;
	int enemyWalkSprites_L3[PREDATOR_WALK_FRAMES];
	int enemyWalkSpritesRight_L3[PREDATOR_WALK_FRAMES];
	// Enemy death sprites - Level 3 (Predators)
	int enemyDeathSprites_L3_Left[PREDATOR_DEATH_FRAMES];
	int enemyDeathSprites_L3_Right[PREDATOR_DEATH_FRAMES];
	// Enemy attack sprites - Level 3 (Predators)
	int enemyAttackSprites_L3[PREDATOR_ATTACK_FRAMES];
	int enemyAttackSpritesRight_L3[PREDATOR_ATTACK_FRAMES];
	// --- Enemy sprites - Level 4 (Demon Boss) ---
	int enemyWalkSprites_L4[DEMON_WALK_FRAMES];
	int enemyWalkSpritesRight_L4[DEMON_WALK_FRAMES];
	// --- Enemy attack sprites - Level 4 (Demon Boss) ---
	int enemyAttackSprites_L4[DEMON_ATTACK_FRAMES];
	int enemyAttackSpritesRight_L4[DEMON_ATTACK_FRAMES];
	// --- Enemy death sprites - Level 4 (Demon Boss) ---
	int enemyDyingSprites_L4[DEMON_DEATH_FRAMES];
	int enemyDyingSpritesRight_L4[DEMON_DEATH_FRAMES];
	// -----------------------------------------
	// --- Add Level Selection Button Images ---
	int level1ButtonImage;
	int level2ButtonImage;
	int level3ButtonImage;
	int level4ButtonImage;
	// -----------------------------------------
};
struct Character {
	// Position and size
	int x;
	int y;
	int width;
	int height;
	// Movement properties
	int walkSpeed;
	int runSpeed;
	int facingDirection;
	// Movement states
	bool moveLeft;
	bool moveRight;
	bool isRunning;
	bool isJumping;
	bool isAttacking;
	bool onGround;
	bool isDead;
	// Animation properties
	CharacterState currentState;
	int currentFrame;
	int animationCounter;
	int animationSpeed;
	// Attack animation
	int attackFrame;
	int attackAnimationCounter;
	int attackAnimationSpeed;
	// Jump physics
	int jumpVelocity;
	int jumpFrame;
	// Health
	int health;
	bool collisionCooldown;
	int collisionTimer;
	// Death animation
	int deathFrame;
	int deathAnimationCounter;
	int deathAnimationSpeed;
};
struct Enemy {
	int x;
	int y;
	int width;
	int height;
	bool active;
	int currentFrame;
	int dyingFrame;
	int animationCounter;
	int animationSpeed;
	EnemyState currentState;
	EnemyDirection facingDirection; // Add direction field
	EnemyType type;
	int hitCount;
	bool alreadyHit;
	bool hasBeenDodged;
	int health;
	bool collisionCooldown;
	int collisionTimer;
	// --- Add Enemy Attack Fields ---
	bool isAttacking;
	int attackFrame;
	int attackAnimationCounter;
	int attackAnimationSpeed;
	int attackCooldownCounter;
	int stunTimer;
	// -------------------------------
};
struct InputState {
	bool keyPressed[256];
	bool specialKeyPressed[256];
};
struct UIElements {
	// Button positions and sizes
	struct Button {
		int x, y, width, height;
		bool isHovered;
	};
	Button playButton;
	Button instructionsButton;
	Button aboutButton;
	Button exitButton;
	Button highScoresButton;
	Button levelButton; // <<< ADDED FOR LEVEL JUMPING
	Button clickButton;
	Button returnButton;
	Button pauseButton;
	// --- Add Level Selection Buttons ---
	Button level1Button;
	Button level2Button;
	Button level3Button;
	Button level4Button;
	// ----------------------------------
};
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              GLOBAL INSTANCES
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GameState gameState;
Images images;
Character character;
Enemy enemies[MAX_ENEMIES];
InputState inputState;
UIElements ui;
// --- Add Music State Flags ---
bool isMenuMusicPlaying = false;
bool isGameMusicPlaying = false;
bool isBossMusicPlaying = false;
// -----------------------------
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              ANIMATION FUNCTIONS
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void updateAnimation()
{
	// Handle death animation separately
	if (character.currentState == STATE_DEATH_ANIM) {
		character.deathAnimationCounter++;
		if (character.deathAnimationCounter >= character.deathAnimationSpeed) {
			character.deathAnimationCounter = 0;
			character.deathFrame++;
			if (character.deathFrame >= 4) {
				character.deathFrame = 3;
				gameState.deathAnimationFinished = true;
			}
		}
		return;
	}
	// Handle attack animation separately with faster speed
	if (character.currentState == STATE_ATTACKING)
	{
		character.attackAnimationCounter++;
		if (character.attackAnimationCounter >= character.attackAnimationSpeed)
		{
			character.attackAnimationCounter = 0;
			character.attackFrame++;
			// Check if attack animation is complete (5 frames: 0-4)
			if (character.attackFrame >= 5)
			{
				character.isAttacking = false;
				character.currentState = STATE_IDLE;
				character.attackFrame = 0;
				character.currentFrame = 0;
				// Reset hit flags when attack ends
				for (int i = 0; i < MAX_ENEMIES; i++) {
					enemies[i].alreadyHit = false;
				}
			}
		}
		return;
	}
	// Regular animation counter for other states
	character.animationCounter++;
	if (character.animationCounter >= character.animationSpeed)
	{
		character.animationCounter = 0;
		if (character.currentState == STATE_WALKING)
		{
			character.currentFrame = (character.currentFrame + 1) % 8;
		}
		else if (character.currentState == STATE_RUNNING)
		{
			character.currentFrame = (character.currentFrame + 1) % 8;
		}
		else if (character.currentState == STATE_JUMPING)
		{
			// Jump animation progresses with jump phases
			if (character.jumpFrame < 11)
			{
				character.currentFrame = character.jumpFrame;
			}
		}
		else if (character.currentState == STATE_DYING)
		{
			// Character dying animation
			character.currentFrame = 0;
		}
	}
}
void updateEnemyAnimation()
{
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active) {
			enemies[i].animationCounter++;
			if (enemies[i].animationCounter >= enemies[i].animationSpeed) {
				enemies[i].animationCounter = 0;
				if (enemies[i].type == ENEMY_TYPE_SKELETON) {
					if (enemies[i].currentState == ENEMY_WALKING) {
						enemies[i].currentFrame = (enemies[i].currentFrame + 1) % 24;
					}
					else if (enemies[i].currentState == ENEMY_STUNNED) {
						enemies[i].currentFrame = 0;
					}
					else if (enemies[i].currentState == ENEMY_DYING) {
						enemies[i].dyingFrame++;
						if (enemies[i].dyingFrame >= 15) {
							enemies[i].active = false;
						}
					}
				}
				// --- Apply Level 1 Logic Structure to Level 2 ---
				else if (enemies[i].type == ENEMY_TYPE_TROLL) {
					if (enemies[i].currentState == ENEMY_WALKING) {
						enemies[i].currentFrame = (enemies[i].currentFrame + 1) % 10;
					}
					else if (enemies[i].currentState == ENEMY_STUNNED) {
						enemies[i].currentFrame = 0;
					}
					else if (enemies[i].currentState == ENEMY_DYING) {
						enemies[i].dyingFrame++;
						// Fixed: Ensure troll death animation plays completely
						if (enemies[i].dyingFrame >= 10) {
							enemies[i].active = false;
						}
					}
				}
				// --- END MODIFICATION ---
				// --- Apply Level 1 Logic Structure to Level 3 (Predator) ---
				else if (enemies[i].type == ENEMY_TYPE_PREDATOR) {
					if (enemies[i].currentState == ENEMY_WALKING) {
						enemies[i].currentFrame = (enemies[i].currentFrame + 1) % PREDATOR_WALK_FRAMES;
					}
					else if (enemies[i].currentState == ENEMY_STUNNED) {
						enemies[i].currentFrame = 0;
					}
					else if (enemies[i].currentState == ENEMY_DYING) {
						enemies[i].dyingFrame++;
						// Fixed: Ensure predator death animation plays completely
						if (enemies[i].dyingFrame >= PREDATOR_DEATH_FRAMES) {
							enemies[i].active = false;
						}
					}
				}
				// --- Apply Level 1 Logic Structure to Level 4 (Demon) ---
				else if (enemies[i].type == ENEMY_TYPE_DEMON) {
					if (enemies[i].currentState == ENEMY_WALKING) {
						enemies[i].currentFrame = (enemies[i].currentFrame + 1) % DEMON_WALK_FRAMES;
					}
					else if (enemies[i].currentState == ENEMY_STUNNED) {
						enemies[i].currentFrame = 0;
						// Handle stun timer in animation function for Level 4 demon
						enemies[i].stunTimer--;
						if (enemies[i].stunTimer <= 0) {
							enemies[i].currentState = ENEMY_WALKING;
							enemies[i].stunTimer = 0;
							enemies[i].attackCooldownCounter = 0; // Reset attack cooldown
						}
					}
					else if (enemies[i].currentState == ENEMY_ATTACKING) {
						enemies[i].attackAnimationCounter++;
						if (enemies[i].attackAnimationCounter >= enemies[i].attackAnimationSpeed) {
							enemies[i].attackAnimationCounter = 0;
							enemies[i].attackFrame++;
							if (enemies[i].attackFrame >= DEMON_ATTACK_FRAMES) {
								enemies[i].isAttacking = false;
								enemies[i].attackFrame = 0;
								enemies[i].currentState = ENEMY_WALKING;
								enemies[i].attackCooldownCounter = 30;
							}
						}
					}
					else if (enemies[i].currentState == ENEMY_DYING) {
						enemies[i].dyingFrame++;
						if (enemies[i].dyingFrame >= DEMON_DEATH_FRAMES) {
							enemies[i].active = false;
						}
					}
				}
			}
		}
	}
}
void updateCharacterState()
{
	if (character.isAttacking)
	{
		character.currentState = STATE_ATTACKING;
		return;
	}
	if (character.isJumping)
	{
		character.currentState = STATE_JUMPING;
	}
}
int getCurrentSprite()
{
	switch (character.currentState)
	{
	case STATE_DEATH_ANIM:
		return images.deathSprites[character.facingDirection][character.deathFrame];
	case STATE_IDLE:
		return images.idleSprite[character.facingDirection];
	case STATE_WALKING:
		return images.walkSprites[character.facingDirection][character.currentFrame];
	case STATE_RUNNING:
		return images.runSprites[character.facingDirection][character.currentFrame];
	case STATE_JUMPING:
		return images.jumpSprites[character.facingDirection][character.currentFrame];
	case STATE_ATTACKING:
		return images.attackSprites[character.facingDirection][character.attackFrame];
	case STATE_DYING:
		return images.dyingSprite[character.facingDirection];
	default:
		return images.idleSprite[character.facingDirection];
	}
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              GAME FUNCTIONS
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void startAttack()
{
	if (character.onGround && !character.isAttacking && !character.isJumping && !character.isDead)
	{
		character.isAttacking = true;
		character.currentState = STATE_ATTACKING;
		character.attackFrame = 0;
		character.attackAnimationCounter = 0;
		for (int i = 0; i < MAX_ENEMIES; i++) {
			enemies[i].alreadyHit = false;
		}
	}
}
void checkAttackCollision()
{
	if (character.currentState == STATE_ATTACKING && character.attackFrame == 2) {
		int hitboxX, hitboxY, hitboxWidth, hitboxHeight;
		const int HITBOX_WIDTH = 20;
		const int HITBOX_HEIGHT = CHARACTER_HEIGHT - 20;
		const int HITBOX_OFFSET_X = 10;
		const int HITBOX_OFFSET_Y = 10;
		if (character.facingDirection == DIRECTION_RIGHT) {
			hitboxX = character.x + character.width + HITBOX_OFFSET_X;
			hitboxY = character.y + HITBOX_OFFSET_Y;
		}
		else {
			hitboxX = character.x - HITBOX_OFFSET_X - HITBOX_WIDTH;
			hitboxY = character.y + HITBOX_OFFSET_Y;
		}
		hitboxWidth = HITBOX_WIDTH;
		hitboxHeight = HITBOX_HEIGHT;
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].active && enemies[i].currentState != ENEMY_DYING && !enemies[i].alreadyHit) {
				if (hitboxX < enemies[i].x + enemies[i].width &&
					hitboxX + hitboxWidth > enemies[i].x &&
					hitboxY < enemies[i].y + enemies[i].height &&
					hitboxY + hitboxHeight > enemies[i].y) {
					enemies[i].hitCount++;
					enemies[i].alreadyHit = true;
					// Special handling for Level 4 boss
					if (enemies[i].type == ENEMY_TYPE_DEMON) {
						enemies[i].health -= (100 / LEVEL_4_HITS_TO_KILL);
						// Check if boss is defeated (15 hits)
						if (enemies[i].hitCount >= LEVEL_4_HITS_TO_KILL || enemies[i].health <= 0) {
							enemies[i].currentState = ENEMY_DYING;
							enemies[i].dyingFrame = 0;
							if (enemies[i].isAttacking) {
								enemies[i].isAttacking = false;
								enemies[i].attackFrame = 0;
							}
							gameState.enemiesKilled++;
							if (gameState.enemiesKilled >= 1) {
								gameState.levelComplete = true;
							}
						}
						else {
							// Stun the boss for a short duration
							enemies[i].currentState = ENEMY_STUNNED;
							enemies[i].stunTimer = LEVEL_4_STUN_DURATION;
							if (enemies[i].isAttacking) {
								enemies[i].isAttacking = false;
								enemies[i].attackFrame = 0;
							}
						}
					}
					// Normal enemy handling
					else {
						enemies[i].health -= 50;
						if (enemies[i].hitCount == HITS_TO_STUN) {
							enemies[i].currentState = ENEMY_STUNNED;
							if (enemies[i].isAttacking) {
								enemies[i].isAttacking = false;
								enemies[i].attackFrame = 0;
							}
						}
						else if (enemies[i].hitCount >= HITS_TO_KILL || enemies[i].health <= 0) {
							enemies[i].currentState = ENEMY_DYING;
							enemies[i].dyingFrame = 0; // Fixed: Reset dying frame to 0 when starting death animation
							if (enemies[i].isAttacking) {
								enemies[i].isAttacking = false;
								enemies[i].attackFrame = 0;
							}
							gameState.enemiesKilled++;
							// Check if level is complete
							if (gameState.enemiesKilled >= ENEMIES_PER_LEVEL) {
								gameState.levelComplete = true;
							}
						}
					}
				}
			}
		}
	}
}
void checkEnemyAttackCollision() {
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].type == ENEMY_TYPE_PREDATOR && enemies[i].active && enemies[i].isAttacking && enemies[i].attackFrame == 2) {
			int attackX, attackY, attackWidth, attackHeight;
			if (enemies[i].facingDirection == ENEMY_DIRECTION_RIGHT) {
				attackX = enemies[i].x + enemies[i].width;
				attackY = enemies[i].y;
				attackWidth = ENEMY_ATTACK_RANGE;
				attackHeight = enemies[i].height;
			}
			else {
				attackX = enemies[i].x - ENEMY_ATTACK_RANGE;
				attackY = enemies[i].y;
				attackWidth = ENEMY_ATTACK_RANGE;
				attackHeight = enemies[i].height;
			}
			if (character.x < attackX + attackWidth &&
				character.x + character.width > attackX &&
				character.y < attackY + attackHeight &&
				character.y + character.height > attackY) {
				if (!character.collisionCooldown) {
					character.health -= ENEMY_ATTACK_DAMAGE;
					gameState.damageTaken += ENEMY_ATTACK_DAMAGE;
					gameState.currentScore -= 50;
					if (gameState.currentScore < 0) gameState.currentScore = 0; // Prevent negative score
					// -----------------------------------------------
					character.collisionCooldown = true;
					character.collisionTimer = 0;
					// Check if character is dead
					if (character.health <= 0) {
						character.health = 0;
						character.isDead = true;
						character.currentState = STATE_DEATH_ANIM;
						character.deathFrame = 0;
						character.deathAnimationCounter = 0;
						gameState.deathAnimationFinished = false;
						gameState.characterDead = true;
					}
				}
			}
		}
		// Add attack collision for Level 4 Demon Boss
		// Add attack collision for Level 4 Demon Boss
		else if (enemies[i].type == ENEMY_TYPE_DEMON && enemies[i].active && enemies[i].isAttacking && enemies[i].attackFrame == 10) {
			int attackX, attackY, attackWidth, attackHeight;
			if (enemies[i].facingDirection == ENEMY_DIRECTION_RIGHT) {
				attackX = enemies[i].x + enemies[i].width;
				attackY = enemies[i].y;
				attackWidth = ENEMY_ATTACK_RANGE; // Use normal attack range for more precise collision
				attackHeight = enemies[i].height;
			}
			else {
				attackX = enemies[i].x - ENEMY_ATTACK_RANGE;
				attackY = enemies[i].y;
				attackWidth = ENEMY_ATTACK_RANGE;
				attackHeight = enemies[i].height;
			}
			if (character.x < attackX + attackWidth &&
				character.x + character.width > attackX &&
				character.y < attackY + attackHeight &&
				character.y + character.height > attackY) {
				if (!character.collisionCooldown) {
					character.health -= (CHARACTER_MAX_HEALTH / LEVEL_4_CHARACTER_DEATH_HITS);
					gameState.damageTaken += (CHARACTER_MAX_HEALTH / LEVEL_4_CHARACTER_DEATH_HITS);
					gameState.currentScore -= 50;
					if (gameState.currentScore < 0) gameState.currentScore = 0;
					// Check if character should die after 4 hits from boss
					if (character.health <= 0) {
						character.health = 0;
						character.isDead = true;
						character.currentState = STATE_DEATH_ANIM;
						character.deathFrame = 0;
						character.deathAnimationCounter = 0;
						gameState.deathAnimationFinished = false;
						gameState.characterDead = true;
					}
					character.collisionCooldown = true;
					character.collisionTimer = 0;
				}
			}
		}
	}
}
void checkEnemyCollision()
{
	if (character.isDead) return;
	if (character.collisionCooldown) {
		character.collisionTimer++;
		if (character.collisionTimer > 30) {
			character.collisionCooldown = false;
			character.collisionTimer = 0;
		}
	}
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active && enemies[i].currentState != ENEMY_DYING) {
			if (enemies[i].collisionCooldown) {
				enemies[i].collisionTimer++;
				if (enemies[i].collisionTimer > 30) {
					enemies[i].collisionCooldown = false;
					enemies[i].collisionTimer = 0;
				}
			}
			if (character.x < enemies[i].x + enemies[i].width &&
				character.x + character.width > enemies[i].x &&
				character.y < enemies[i].y + enemies[i].height &&
				character.y + character.height > enemies[i].y) {
				if (!character.collisionCooldown) {
					// Special handling for Level 4 boss collision
					// Level 4 boss doesn't deal collision damage - only attack damage
					if (enemies[i].type == ENEMY_TYPE_DEMON) {
						// No collision damage for Level 4 boss
						continue;
					}
					// Normal enemy collision handling
					else {
						character.health -= COLLISION_DAMAGE;
						gameState.damageTaken += COLLISION_DAMAGE;
						gameState.currentScore -= 50;
						if (gameState.currentScore < 0) gameState.currentScore = 0;
					}
					character.collisionCooldown = true;
					character.collisionTimer = 0;
					enemies[i].collisionCooldown = true;
					enemies[i].collisionTimer = 0;
					// Check if character is dead (for normal enemies too)
					if (character.health <= 0 && enemies[i].type != ENEMY_TYPE_DEMON) {
						character.health = 0;
						character.isDead = true;
						character.currentState = STATE_DEATH_ANIM;
						character.deathFrame = 0;
						character.deathAnimationCounter = 0;
						gameState.deathAnimationFinished = false;
						gameState.characterDead = true;
					}
				}
			}
		}
	}
}
void spawnEnemies()
{
	gameState.enemiesSpawnedInLevel = 0;
	gameState.enemiesKilled = 0;
	gameState.levelComplete = false;
	gameState.awaitingLevelProgression = false;
	gameState.nextEnemyToSpawn = 0;
	EnemyType currentEnemyType = ENEMY_TYPE_SKELETON;
	if (gameState.currentLevel == LEVEL_1) {
		currentEnemyType = ENEMY_TYPE_SKELETON;
	}
	else if (gameState.currentLevel == LEVEL_2) {
		currentEnemyType = ENEMY_TYPE_TROLL;
	}
	else if (gameState.currentLevel == LEVEL_3) {
		currentEnemyType = ENEMY_TYPE_PREDATOR;
	}
	else if (gameState.currentLevel == LEVEL_4) { // Added for Level 4
		currentEnemyType = ENEMY_TYPE_DEMON;
	}
	// For Level 4, spawn only 1 enemy (the boss)
	int enemiesToSpawn;
	if (gameState.currentLevel == LEVEL_4) {
		enemiesToSpawn = 1; // Only spawn 1 boss
	}
	else {
		enemiesToSpawn = (ENEMIES_PER_WAVE < ENEMIES_PER_LEVEL) ? ENEMIES_PER_WAVE : ENEMIES_PER_LEVEL;
	}
	for (int i = 0; i < enemiesToSpawn && gameState.enemiesSpawnedInLevel < (gameState.currentLevel == LEVEL_4 ? 1 : ENEMIES_PER_LEVEL); i++) {
		enemies[i].active = true;
		enemies[i].x = WINDOW_WIDTH + 200 + (i * 400);
		enemies[i].y = GROUND_Y;
		// Set enemy size based on level
		if (currentEnemyType == ENEMY_TYPE_DEMON) {
			enemies[i].width = LEVEL_4_ENEMY_WIDTH;
			enemies[i].height = LEVEL_4_ENEMY_HEIGHT;
		}
		else {
			enemies[i].width = ENEMY_WIDTH;
			enemies[i].height = ENEMY_HEIGHT;
		}
		enemies[i].currentFrame = 0;
		enemies[i].dyingFrame = 0;
		enemies[i].animationCounter = 0;
		enemies[i].animationSpeed = 5;
		enemies[i].currentState = ENEMY_WALKING;
		enemies[i].facingDirection = ENEMY_DIRECTION_LEFT;
		enemies[i].type = currentEnemyType;
		enemies[i].hitCount = 0;
		enemies[i].alreadyHit = false;
		enemies[i].hasBeenDodged = false;
		// Set health for Level 4 boss
		if (currentEnemyType == ENEMY_TYPE_DEMON) {
			enemies[i].health = ENEMY_MAX_HEALTH; // Full health for boss
		}
		else {
			enemies[i].health = ENEMY_MAX_HEALTH;
		}
		enemies[i].collisionCooldown = false;
		enemies[i].collisionTimer = 0;
		gameState.enemiesSpawnedInLevel++;
		gameState.nextEnemyToSpawn++;
		enemies[i].isAttacking = false;
		enemies[i].attackFrame = 0;
		enemies[i].attackAnimationCounter = 0;
		enemies[i].attackAnimationSpeed = 1; // Changed from 3 to 1 for faster animation
		enemies[i].attackCooldownCounter = rand() % ENEMY_ATTACK_COOLDOWN;
		enemies[i].stunTimer = 0;
	}
	for (int i = enemiesToSpawn; i < MAX_ENEMIES; i++) {
		enemies[i].active = false;
		enemies[i].hitCount = 0;
		enemies[i].alreadyHit = false;
		enemies[i].hasBeenDodged = false;
		enemies[i].health = ENEMY_MAX_HEALTH;
		enemies[i].collisionCooldown = false;
		enemies[i].collisionTimer = 0;
		enemies[i].facingDirection = ENEMY_DIRECTION_LEFT;
		enemies[i].type = currentEnemyType;
		enemies[i].isAttacking = false;
		enemies[i].attackFrame = 0;
		enemies[i].attackAnimationCounter = 0;
		enemies[i].attackAnimationSpeed = 3;
		enemies[i].attackCooldownCounter = 0;
		enemies[i].stunTimer = 0;
		// ------------------------------------------
	}
	gameState.enemiesSpawned = true;
}
void updateEnemies()
{
	for (int i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active) {
			if (enemies[i].attackCooldownCounter > 0) {
				enemies[i].attackCooldownCounter--;
			}
			if (enemies[i].currentState == ENEMY_DYING) {
				// Don't reset frame here - let animation play through
			}
			else if (enemies[i].currentState == ENEMY_STUNNED) {
				enemies[i].currentFrame = 0;
				if (enemies[i].isAttacking) {
					enemies[i].isAttacking = false;
					enemies[i].attackFrame = 0;
				}
			}
			else if (enemies[i].type == ENEMY_TYPE_SKELETON) {
				int distanceToCharacter = abs(enemies[i].x - character.x);
				if (distanceToCharacter > MIN_DISTANCE_TO_CHARACTER) {
					if (enemies[i].x > character.x + MIN_DISTANCE_TO_CHARACTER) {
						enemies[i].x -= ENEMY_SPEED;
						enemies[i].facingDirection = ENEMY_DIRECTION_LEFT;
					}
					else if (enemies[i].x < character.x - MIN_DISTANCE_TO_CHARACTER) {
						enemies[i].x += ENEMY_SPEED;
						enemies[i].facingDirection = ENEMY_DIRECTION_RIGHT;
					}
				}
				if (enemies[i].x < -50) {
					enemies[i].x = -50;
				}
				else if (enemies[i].x > WINDOW_WIDTH + 50) {
					enemies[i].x = WINDOW_WIDTH + 50;
				}
				enemies[i].currentState = ENEMY_WALKING;
			}
			else if (enemies[i].type == ENEMY_TYPE_TROLL) {
				int distanceToCharacter = abs(enemies[i].x - character.x);
				if (distanceToCharacter > MIN_DISTANCE_TO_CHARACTER) {
					if (enemies[i].x > character.x + MIN_DISTANCE_TO_CHARACTER) {
						enemies[i].x -= ENEMY_SPEED;
						enemies[i].facingDirection = ENEMY_DIRECTION_LEFT;
					}
					else if (enemies[i].x < character.x - MIN_DISTANCE_TO_CHARACTER) {
						enemies[i].x += ENEMY_SPEED;
						enemies[i].facingDirection = ENEMY_DIRECTION_RIGHT;
					}
				}
				if (enemies[i].x < -50) {
					enemies[i].x = -50;
				}
				else if (enemies[i].x > WINDOW_WIDTH + 50) {
					enemies[i].x = WINDOW_WIDTH + 50;
				}
				enemies[i].currentState = ENEMY_WALKING;
			}
			else if (enemies[i].type == ENEMY_TYPE_PREDATOR) {
				if (enemies[i].currentState == ENEMY_STUNNED) {
					continue;
				}
				if (enemies[i].isAttacking) {
					int distanceToCharacter = abs(enemies[i].x - character.x);
					if (distanceToCharacter > ENEMY_ATTACK_RANGE + 50) {
						enemies[i].isAttacking = false;
						enemies[i].attackFrame = 0;
						if (enemies[i].currentState == ENEMY_ATTACKING && enemies[i].hitCount < HITS_TO_STUN) {
							enemies[i].currentState = ENEMY_WALKING;
						}
					}
					continue;
				}
				if (enemies[i].currentState == ENEMY_WALKING) {
					int distanceToCharacter = abs(enemies[i].x - character.x);
					if (distanceToCharacter <= ENEMY_ATTACK_RANGE &&
						enemies[i].attackCooldownCounter <= 0 &&
						!enemies[i].isAttacking &&
						enemies[i].hitCount < HITS_TO_STUN) {
						if (enemies[i].x > character.x + 10) {
							enemies[i].facingDirection = ENEMY_DIRECTION_LEFT;
						}
						else if (enemies[i].x < character.x - 10) {
							enemies[i].facingDirection = ENEMY_DIRECTION_RIGHT;
						}
						if (enemies[i].type == ENEMY_TYPE_PREDATOR) {
							enemies[i].isAttacking = true;
							enemies[i].attackFrame = 0;
							enemies[i].attackAnimationCounter = 0;
							enemies[i].currentState = ENEMY_ATTACKING;
						}
					}
					else {
						int distanceToCharacter = abs(enemies[i].x - character.x);
						if (distanceToCharacter > MIN_DISTANCE_TO_CHARACTER) {
							if (enemies[i].x > character.x + MIN_DISTANCE_TO_CHARACTER) {
								enemies[i].x -= ENEMY_SPEED;
								enemies[i].facingDirection = ENEMY_DIRECTION_LEFT;
							}
							else if (enemies[i].x < character.x - MIN_DISTANCE_TO_CHARACTER) {
								enemies[i].x += ENEMY_SPEED;
								enemies[i].facingDirection = ENEMY_DIRECTION_RIGHT;
							}
						}
					}
					if (enemies[i].x < -50) {
						enemies[i].x = -50;
					}
					else if (enemies[i].x > WINDOW_WIDTH + 50) {
						enemies[i].x = WINDOW_WIDTH + 50;
					}
				}
			}
			// --- Add Level 4 Enemy (Demon) Behavior ---
			// --- Add Level 4 Enemy (Demon) Behavior ---
			// --- Add Level 4 Enemy (Demon) Behavior ---
			// --- Add Level 4 Enemy (Demon) Behavior ---
			else if (enemies[i].type == ENEMY_TYPE_DEMON) {
				if (enemies[i].currentState == ENEMY_STUNNED) {
					enemies[i].stunTimer--;
					if (enemies[i].stunTimer <= 0) {
						enemies[i].currentState = ENEMY_WALKING;
						enemies[i].stunTimer = 0;
						enemies[i].attackCooldownCounter = 0;
					}
				}
				else if (enemies[i].isAttacking) {
					int distanceToCharacter = abs(enemies[i].x - character.x);
					if (distanceToCharacter > ENEMY_ATTACK_RANGE + 100) {
						enemies[i].isAttacking = false;
						enemies[i].attackFrame = 0;
						enemies[i].currentState = ENEMY_WALKING;
					}
				}
				else if (enemies[i].currentState == ENEMY_WALKING) {
					int distanceToCharacter = abs(enemies[i].x - character.x);
					if (distanceToCharacter <= ENEMY_ATTACK_RANGE + 50 &&
						enemies[i].attackCooldownCounter <= 0 &&
						!enemies[i].isAttacking &&
						enemies[i].hitCount < LEVEL_4_HITS_TO_KILL &&
						!character.isDead) {
						if (enemies[i].x > character.x + 10) {
							enemies[i].facingDirection = ENEMY_DIRECTION_LEFT;
						}
						else if (enemies[i].x < character.x - 10) {
							enemies[i].facingDirection = ENEMY_DIRECTION_RIGHT;
						}
						enemies[i].isAttacking = true;
						enemies[i].attackFrame = 0;
						enemies[i].attackAnimationCounter = 0;
						enemies[i].currentState = ENEMY_ATTACKING;
					}
					else {
						if (distanceToCharacter > MIN_DISTANCE_TO_CHARACTER) {
							if (enemies[i].x > character.x + MIN_DISTANCE_TO_CHARACTER) {
								enemies[i].x -= ENEMY_SPEED;
								enemies[i].facingDirection = ENEMY_DIRECTION_LEFT;
							}
							else if (enemies[i].x < character.x - MIN_DISTANCE_TO_CHARACTER) {
								enemies[i].x += ENEMY_SPEED;
								enemies[i].facingDirection = ENEMY_DIRECTION_RIGHT;
							}
						}
					}
					if (enemies[i].x < -50) {
						enemies[i].x = -50;
					}
					else if (enemies[i].x > WINDOW_WIDTH + 50) {
						enemies[i].x = WINDOW_WIDTH + 50;
					}
				}
			}
		}
	}
	// Special handling for Level 4 - only 1 enemy, no waves
	if (gameState.currentLevel == LEVEL_4) {
		return; // Skip the wave spawning logic for Level 4
	}
	if (gameState.enemiesSpawned && !gameState.levelComplete) {
		bool  allActiveEnemiesDeadOrDying = true;
		int activeEnemiesCount = 0;
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].active) {
				activeEnemiesCount++;
				if (enemies[i].currentState != ENEMY_DYING) {
					allActiveEnemiesDeadOrDying = false;
					break;
				}
			}
		}
		if (allActiveEnemiesDeadOrDying && gameState.enemiesSpawnedInLevel < ENEMIES_PER_LEVEL) {
			int enemiesToSpawn = ENEMIES_PER_WAVE;
			if (gameState.enemiesSpawnedInLevel + enemiesToSpawn > ENEMIES_PER_LEVEL) {
				enemiesToSpawn = ENEMIES_PER_LEVEL - gameState.enemiesSpawnedInLevel;
			}
			for (int i = 0; i < enemiesToSpawn; i++) {
				if (gameState.nextEnemyToSpawn < MAX_ENEMIES) {
					enemies[gameState.nextEnemyToSpawn].active = true;
					enemies[gameState.nextEnemyToSpawn].x = WINDOW_WIDTH + 400;
					enemies[gameState.nextEnemyToSpawn].y = GROUND_Y;
					// Set enemy size based on level
					if (enemies[gameState.nextEnemyToSpawn].type == ENEMY_TYPE_DEMON) {
						enemies[gameState.nextEnemyToSpawn].width = LEVEL_4_ENEMY_WIDTH;
						enemies[gameState.nextEnemyToSpawn].height = LEVEL_4_ENEMY_HEIGHT;
					}
					else {
						enemies[gameState.nextEnemyToSpawn].width = ENEMY_WIDTH;
						enemies[gameState.nextEnemyToSpawn].height = ENEMY_HEIGHT;
					}
					enemies[gameState.nextEnemyToSpawn].currentFrame = 0;
					enemies[gameState.nextEnemyToSpawn].dyingFrame = 0;
					enemies[gameState.nextEnemyToSpawn].animationCounter = 0;
					enemies[gameState.nextEnemyToSpawn].animationSpeed = 5;
					enemies[gameState.nextEnemyToSpawn].currentState = ENEMY_WALKING;
					enemies[gameState.nextEnemyToSpawn].facingDirection = ENEMY_DIRECTION_LEFT;
					enemies[gameState.nextEnemyToSpawn].hitCount = 0;
					enemies[gameState.nextEnemyToSpawn].alreadyHit = false;
					enemies[gameState.nextEnemyToSpawn].hasBeenDodged = false;
					enemies[gameState.nextEnemyToSpawn].health = ENEMY_MAX_HEALTH;
					enemies[gameState.nextEnemyToSpawn].collisionCooldown = false;
					enemies[gameState.nextEnemyToSpawn].collisionTimer = 0;
					enemies[gameState.nextEnemyToSpawn].isAttacking = false;
					enemies[gameState.nextEnemyToSpawn].attackFrame = 0;
					enemies[gameState.nextEnemyToSpawn].attackAnimationCounter = 0;
					enemies[gameState.nextEnemyToSpawn].attackAnimationSpeed = 3;
					enemies[gameState.nextEnemyToSpawn].attackCooldownCounter = rand() % ENEMY_ATTACK_COOLDOWN;
					// ------------------------------------------------------------
					gameState.enemiesSpawnedInLevel++;
					gameState.nextEnemyToSpawn++;
				}
			}
		}
	}
}
void updateCharacter()
{
	if (character.isAttacking)
	{
		updateCharacterState();
		updateAnimation();
		checkAttackCollision();
		return;
	}
	checkEnemyCollision();
	checkEnemyAttackCollision();
	if (character.isJumping)
	{
		character.y += character.jumpVelocity;
		character.jumpVelocity -= GRAVITY;
		int jumpProgress = (25 - character.jumpVelocity) / 2;
		if (jumpProgress < 0) jumpProgress = 0;
		if (jumpProgress > 11) jumpProgress = 11;
		character.jumpFrame = jumpProgress;
		if (character.y <= GROUND_Y)
		{
			character.y = GROUND_Y;
			character.isJumping = false;
			character.onGround = true;
			character.jumpVelocity = 0;
			character.jumpFrame = 0;
			character.currentState = STATE_IDLE;
			character.currentFrame = 0;
			inputState.keyPressed['w'] = false;
			inputState.keyPressed['W'] = false;
			inputState.keyPressed['a'] = false;
			inputState.keyPressed['A'] = false;
			inputState.keyPressed['d'] = false;
			inputState.keyPressed['D'] = false;
		}
	}
	updateCharacterState();
	if (character.currentState == STATE_JUMPING)
	{
		updateAnimation();
	}
	if (character.currentState == STATE_DEATH_ANIM) {
		updateAnimation();
	}
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              HEALTH BAR FUNCTIONS
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void drawCharacterHealthBar()
{
	if (character.isDead) return;
	int barWidth = 100;
	int barHeight = 15;
	int barX = character.x - 2;
	int barY = character.y + character.height + 10;
	iSetColor(255, 0, 0);
	iFilledRectangle(barX, barY, barWidth, barHeight);
	float healthPercent = (float)character.health / CHARACTER_MAX_HEALTH;
	iSetColor(0, 255, 0);
	iFilledRectangle(barX, barY, barWidth * healthPercent, barHeight);
	iSetColor(0, 0, 0);
	iRectangle(barX, barY, barWidth, barHeight);
}
void drawEnemyHealthBar(int enemyIndex)
{
	if (!enemies[enemyIndex].active || enemies[enemyIndex].currentState == ENEMY_DYING) return;
	int barWidth = 100;
	int barHeight = 12;
	int barX = enemies[enemyIndex].x - 2;
	int barY = enemies[enemyIndex].y + enemies[enemyIndex].height + 8;
	iSetColor(255, 0, 0);
	iFilledRectangle(barX, barY, barWidth, barHeight);
	float healthPercent = (float)enemies[enemyIndex].health / ENEMY_MAX_HEALTH;
	iSetColor(0, 255, 0);
	iFilledRectangle(barX, barY, barWidth * healthPercent, barHeight);
	iSetColor(0, 0, 0);
	iRectangle(barX, barY, barWidth, barHeight);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                                  iDraw
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void drawButton(const UIElements::Button& button, int imageId)
{
	iShowImage(
		button.x - (button.isHovered ? 10 : 0),
		button.y - (button.isHovered ? 10 : 0),
		button.width + (button.isHovered ? 20 : 0),
		button.height + (button.isHovered ? 20 : 0),
		imageId
		);
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              SCORING FUNCTIONS
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void saveScore(int score)
{
	FILE *file = fopen("scores.txt", "a");
	if (file != NULL) {
		fprintf(file, "%d", score);
		fclose(file);
	}
}
void loadAndDisplayScores()
{
	FILE *file = fopen("scores.txt", "r"); // Open file in read mode
	if (file != NULL) {
		int scores[100];
		int scoreCount = 0;
		int score;
		while (fscanf(file, "%d", &score) != EOF && scoreCount < 100) {
			scores[scoreCount] = score;
			scoreCount++;
		}
		fclose(file);
		// Sort scores in descending order (Highest to Lowest)
		for (int i = 0; i < scoreCount - 1; i++) {
			for (int j = 0; j < scoreCount - i - 1; j++) {
				if (scores[j] < scores[j + 1]) {
					int temp = scores[j];
					scores[j] = scores[j + 1];
					scores[j + 1] = temp;
				}
			}
		}
		iSetColor(255, 255, 255);
		iText(50, 550, "HIGH SCORES:", GLUT_BITMAP_TIMES_ROMAN_24);
		int yPosition = 500;
		int displayCount = (scoreCount < 10) ? scoreCount : 10;
		for (int i = 0; i < displayCount; i++) {
			char scoreText[50];
			sprintf(scoreText, "%d. %d", i + 1, scores[i]);
			iText(50, yPosition, scoreText, GLUT_BITMAP_HELVETICA_18);
			yPosition -= 30;
		}
	}
	else {
		// File doesn't exist or can't be opened
		iSetColor(255, 255, 255);
		iText(50, 500, "No scores recorded yet.", GLUT_BITMAP_HELVETICA_18);
	}
}
void iDraw()
{
	iClear();
	if (gameState.showMenu)
	{
		iShowImage(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, images.backGround);
		drawButton(ui.playButton, images.playButtonImage);
		drawButton(ui.instructionsButton, images.instructionsButtonImage);
		drawButton(ui.aboutButton, images.aboutButtonImage);
		drawButton(ui.highScoresButton, images.highScoresButtonImage);
		drawButton(ui.levelButton, images.levelButtonImage); // <<< ADDED FOR LEVEL JUMPING
		drawButton(ui.exitButton, images.exitButtonImage);
	}
	else if (gameState.showHighScores)
	{
		iShowImage(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, images.backGround);
		drawButton(ui.returnButton, images.returnButtonImage);
		loadAndDisplayScores();
	}
	// --------------------------------
	// Add Level Selection Screen
	else if (gameState.showLevelSelect)
	{
		iShowImage(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, images.backGround);
		drawButton(ui.returnButton, images.returnButtonImage);
		// Draw Level buttons using persistent button instances
		drawButton(ui.level1Button, images.level1ButtonImage);
		drawButton(ui.level2Button, images.level2ButtonImage);
		drawButton(ui.level3Button, images.level3ButtonImage);
		drawButton(ui.level4Button, images.level4ButtonImage);
	}
	// --------------------------------
	else if (gameState.showInstructions)
	{
		iShowImage(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, images.backGround);
		drawButton(ui.returnButton, images.returnButtonImage);
		iSetColor(255, 255, 255);
		iText(50, 500, "INSTRUCTIONS:", GLUT_BITMAP_TIMES_ROMAN_24);
		iText(50, 470, "A/D: Move left/right", GLUT_BITMAP_HELVETICA_18);
		iText(50, 440, "W: Jump", GLUT_BITMAP_HELVETICA_18);
		iText(50, 410, "F: Attack", GLUT_BITMAP_HELVETICA_18);
		iText(50, 380, "Shift + Movement: Run", GLUT_BITMAP_HELVETICA_18);
		iText(50, 350, "Backspace: Return to menu", GLUT_BITMAP_HELVETICA_18);
		iText(50, 320, "ESC: Pause game / Return to menu", GLUT_BITMAP_HELVETICA_18);
	}
	else if (gameState.showAbout)
	{
		iShowImage(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, images.backGround);
		drawButton(ui.returnButton, images.returnButtonImage);
		iSetColor(255, 255, 255);
		iText(50, 500, "ABOUT US:", GLUT_BITMAP_TIMES_ROMAN_24);
		iText(50, 470, "Name: Fahim Mubtashim", GLUT_BITMAP_HELVETICA_18);
		iText(50, 440, "ID: 00724105101147", GLUT_BITMAP_HELVETICA_18);
		iText(50, 410, "Name: Raiyan Tajuddin Ahmed", GLUT_BITMAP_HELVETICA_18);
		iText(50, 380, "ID: 00724105101142", GLUT_BITMAP_HELVETICA_18);
		iText(50, 350, "Name: Ahmed Nazmus Sakib", GLUT_BITMAP_HELVETICA_18);
		iText(50, 320, "ID: 00724105101154", GLUT_BITMAP_HELVETICA_18);
	}
	else if (gameState.gameStarted)
	{
		iShowImage(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, images.gameBackGround);
		drawButton(ui.returnButton, images.returnButtonImage);
		if (gameState.isPaused) {
			drawButton(ui.pauseButton, images.resumeButtonImage);
		}
		else {
			drawButton(ui.pauseButton, images.pauseButtonImage);
		}
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].active) {
				int sprite;
				if (enemies[i].type == ENEMY_TYPE_SKELETON) {
					if (enemies[i].currentState == ENEMY_WALKING) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyWalkSprites_L1[enemies[i].currentFrame];
						}
						else {
							sprite = images.enemyWalkSpritesRight_L1[enemies[i].currentFrame];
						}
					}
					else if (enemies[i].currentState == ENEMY_STUNNED) {
						sprite = images.enemyStunnedSprite_L1;
					}
					else if (enemies[i].currentState == ENEMY_DYING) {
						sprite = images.enemyDyingSprites_L1[enemies[i].dyingFrame];
					}
				}
				else if (enemies[i].type == ENEMY_TYPE_TROLL) {
					if (enemies[i].currentState == ENEMY_WALKING) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyWalkSprites_L2[enemies[i].currentFrame];
						}
						else {
							sprite = images.enemyWalkSpritesRight_L2[enemies[i].currentFrame];
						}
					}
					else if (enemies[i].currentState == ENEMY_STUNNED) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyStunnedSprite_L2_Left;
						}
						else {
							sprite = images.enemyStunnedSprite_L2_Right;
						}
					}
					else if (enemies[i].currentState == ENEMY_DYING) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyDyingSprites_L2_Left[enemies[i].dyingFrame];
						}
						else {
							sprite = images.enemyDyingSprites_L2_Right[enemies[i].dyingFrame];
						}
					}
				}
				else if (enemies[i].type == ENEMY_TYPE_PREDATOR) {
					if (enemies[i].currentState == ENEMY_WALKING) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyWalkSprites_L3[enemies[i].currentFrame];
						}
						else {
							sprite = images.enemyWalkSpritesRight_L3[enemies[i].currentFrame];
						}
					}
					else if (enemies[i].currentState == ENEMY_STUNNED) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyIdleSprite_L3_Left;
						}
						else {
							sprite = images.enemyIdleSprite_L3_Right;
						}
					}
					else if (enemies[i].currentState == ENEMY_DYING) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyDeathSprites_L3_Left[enemies[i].dyingFrame];
						}
						else {
							sprite = images.enemyDeathSprites_L3_Right[enemies[i].dyingFrame];
						}
					}
					else if (enemies[i].currentState == ENEMY_ATTACKING) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyAttackSprites_L3[enemies[i].attackFrame];
						}
						else {
							sprite = images.enemyAttackSpritesRight_L3[enemies[i].attackFrame];
						}
					}
				}
				// --- Add Level 4 (Demon) Rendering ---
				else if (enemies[i].type == ENEMY_TYPE_DEMON) {
					if (enemies[i].currentState == ENEMY_WALKING) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyWalkSprites_L4[enemies[i].currentFrame];
						}
						else {
							sprite = images.enemyWalkSpritesRight_L4[enemies[i].currentFrame];
						}
					}
					else if (enemies[i].currentState == ENEMY_STUNNED) {
						// Use first frame for stunned state
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyWalkSprites_L4[0];
						}
						else {
							sprite = images.enemyWalkSpritesRight_L4[0];
						}
					}
					else if (enemies[i].currentState == ENEMY_ATTACKING) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyAttackSprites_L4[enemies[i].attackFrame];
						}
						else {
							sprite = images.enemyAttackSpritesRight_L4[enemies[i].attackFrame];
						}
					}
					else if (enemies[i].currentState == ENEMY_DYING) {
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyDyingSprites_L4[enemies[i].dyingFrame];
						}
						else {
							sprite = images.enemyDyingSpritesRight_L4[enemies[i].dyingFrame];
						}
					}
					else {
						// Default to walking first frame
						if (enemies[i].facingDirection == ENEMY_DIRECTION_LEFT) {
							sprite = images.enemyWalkSprites_L4[0];
						}
						else {
							sprite = images.enemyWalkSpritesRight_L4[0];
						}
					}
				}
				iShowImage(enemies[i].x, enemies[i].y,
					enemies[i].width, enemies[i].height,
					sprite);
				drawEnemyHealthBar(i);
			}
		}
		int currentSprite = getCurrentSprite();
		iShowImage(character.x, character.y, character.width, character.height, currentSprite);
		drawCharacterHealthBar();
		if (gameState.characterDead && gameState.deathAnimationFinished) {
			iSetColor(255, 0, 0);
			iText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, "GAME OVER!", GLUT_BITMAP_TIMES_ROMAN_24);
			char scoreText[50];
			sprintf(scoreText, "Final Score: %d", gameState.currentScore);
			iText(WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 - 60, scoreText, GLUT_BITMAP_HELVETICA_18);
			// --------------------------------------------
			iText(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 30, "Press ESC to return to menu", GLUT_BITMAP_HELVETICA_18);
		}
		if (gameState.levelComplete && !gameState.characterDead) {
			if (gameState.currentLevel == LEVEL_4) { // Added for Level 4 completion
				iSetColor(255, 215, 0);
				iText(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2, "CONGRATULATIONS, YOU WON.", GLUT_BITMAP_TIMES_ROMAN_24);
				char scoreText[50];
				sprintf(scoreText, "Final Score: %d", gameState.currentScore);
				iText(WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 - 60, scoreText, GLUT_BITMAP_HELVETICA_18);
				// ----------------------------------------
				iText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 - 30, "PRESS Esc TO RETURN TO MAIN MENU", GLUT_BITMAP_HELVETICA_18);
			}
			else if (gameState.currentLevel == LEVEL_3) {
				iSetColor(255, 215, 0);
				iText(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2, "LEVEL 3 COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
				iText(WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 30, "PRESS SPACE TO START LEVEL 4", GLUT_BITMAP_HELVETICA_18);
			}
			else {
				// Show normal level complete message for level 1 and 2
				iSetColor(0, 255, 0);
				iText(WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2, "LEVEL COMPLETE!", GLUT_BITMAP_TIMES_ROMAN_24);
				iText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 - 30, "Press SPACE to continue to next level", GLUT_BITMAP_HELVETICA_18);
			}
		}
		if (gameState.isPaused) {
			iSetColor(255, 255, 255);
			iText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
		}
	}
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              Mouse Handling
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool isPointInButton(int mx, int my, const UIElements::Button& button)
{
	return mx >= button.x && mx <= button.x + button.width &&
		my >= button.y && my <= button.y + button.height;
}
void resetCharacterToDefault()
{
	character.x = 100;
	character.y = GROUND_Y;
	character.onGround = true;
	character.isJumping = false;
	character.isAttacking = false;
	character.jumpVelocity = 0;
	character.currentState = STATE_IDLE;
	character.currentFrame = 0;
	character.attackFrame = 0;
	character.facingDirection = DIRECTION_RIGHT;
	character.isDead = false;
	character.health = CHARACTER_MAX_HEALTH;
	character.collisionCooldown = false;
	character.collisionTimer = 0;
	character.deathFrame = 0;
	character.deathAnimationCounter = 0;
	character.deathAnimationSpeed = 5;
}
void iMouse(int button, int state, int mx, int my)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (gameState.showMenu)
		{
			if (isPointInButton(mx, my, ui.playButton))
			{
				if (isMenuMusicPlaying) {
					PlaySound(NULL, NULL, SND_PURGE);
					isMenuMusicPlaying = false;
				}
				if (PlaySound("Music\\Battle Theme II v1.2.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
					isGameMusicPlaying = true;
				}
				else {
					isGameMusicPlaying = false;
				}
				gameState.showMenu = false;
				gameState.gameStarted = true;
				gameState.enemiesSpawned = false;
				gameState.characterDead = false;
				gameState.currentLevel = LEVEL_1;
				gameState.currentScore = 0;
				gameState.damageTaken = 0;
				resetCharacterToDefault();
				for (int i = 0; i < MAX_ENEMIES; i++) {
					enemies[i].active = false;
				}
				images.gameBackGround = iLoadImage("Images/New Background.jpg");
			}
			else if (isPointInButton(mx, my, ui.instructionsButton))
			{
				gameState.showMenu = false;
				gameState.showInstructions = true;
			}
			else if (isPointInButton(mx, my, ui.aboutButton))
			{
				gameState.showMenu = false;
				gameState.showAbout = true;
			}
			else if (isPointInButton(mx, my, ui.highScoresButton))
			{
				gameState.showMenu = false;
				gameState.showHighScores = true;
			}
			// Handle Level Button Click
			else if (isPointInButton(mx, my, ui.levelButton))
			{
				gameState.showMenu = false;
				gameState.showLevelSelect = true;
			}
			// ------------------------------------------
			else if (isPointInButton(mx, my, ui.exitButton))
			{
				exit(0);
			}
		}
		// Handle Level Selection Clicks
		else if (gameState.showLevelSelect)
		{
			if (isPointInButton(mx, my, ui.returnButton))
			{
				gameState.showLevelSelect = false;
				gameState.showMenu = true;
			}
			else
			{
				if (isPointInButton(mx, my, ui.level1Button))
				{
					// Start Game at Level 1
					if (isMenuMusicPlaying) {
						PlaySound(NULL, NULL, SND_PURGE);
						isMenuMusicPlaying = false;
					}
					if (PlaySound("Music\\Battle Theme II v1.2.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
						isGameMusicPlaying = true;
					}
					else {
						isGameMusicPlaying = false;
					}
					gameState.showLevelSelect = false;
					gameState.gameStarted = true;
					gameState.enemiesSpawned = false;
					gameState.characterDead = false;
					gameState.currentLevel = LEVEL_1;
					gameState.currentScore = 0;
					gameState.damageTaken = 0;
					resetCharacterToDefault();
					for (int i = 0; i < MAX_ENEMIES; i++) {
						enemies[i].active = false;
					}
					images.gameBackGround = iLoadImage("Images/New Background.jpg");
				}
				else if (isPointInButton(mx, my, ui.level2Button))
				{
					// Start Game at Level 2
					if (isMenuMusicPlaying) {
						PlaySound(NULL, NULL, SND_PURGE);
						isMenuMusicPlaying = false;
					}
					if (PlaySound("Music\\Battle Theme II v1.2.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
						isGameMusicPlaying = true;
					}
					else {
						isGameMusicPlaying = false;
					}
					gameState.showLevelSelect = false;
					gameState.gameStarted = true;
					gameState.enemiesSpawned = false;
					gameState.characterDead = false;
					gameState.currentLevel = LEVEL_2;
					gameState.currentScore = 0;
					gameState.damageTaken = 0;
					resetCharacterToDefault();
					for (int i = 0; i < MAX_ENEMIES; i++) {
						enemies[i].active = false;
					}
					images.gameBackGround = iLoadImage("Images/dead forest.png");
				}
				else if (isPointInButton(mx, my, ui.level3Button))
				{
					// Start Game at Level 3
					if (isMenuMusicPlaying) {
						PlaySound(NULL, NULL, SND_PURGE);
						isMenuMusicPlaying = false;
					}
					if (PlaySound("Music\\Battle Theme II v1.2.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
						isGameMusicPlaying = true;
					}
					else {
						isGameMusicPlaying = false;
					}
					gameState.showLevelSelect = false;
					gameState.gameStarted = true;
					gameState.enemiesSpawned = false;
					gameState.characterDead = false;
					gameState.currentLevel = LEVEL_3;
					gameState.currentScore = 0;
					gameState.damageTaken = 0;
					resetCharacterToDefault();
					for (int i = 0; i < MAX_ENEMIES; i++) {
						enemies[i].active = false;
					}
					images.gameBackGround = iLoadImage("Images/2_game_background.png");
				}
				// --- Handle Level 4 Button Click ---
				else if (isPointInButton(mx, my, ui.level4Button))
				{
					// Start Game at Level 4
					if (isMenuMusicPlaying) {
						PlaySound(NULL, NULL, SND_PURGE);
						isMenuMusicPlaying = false;
					}
					if (PlaySound("Music\\BOSS MUSIC.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
						isBossMusicPlaying = true;
						isGameMusicPlaying = false;
					}
					else {
						isBossMusicPlaying = false;
						isGameMusicPlaying = false;
					}
					gameState.showLevelSelect = false;
					gameState.gameStarted = true;
					gameState.enemiesSpawned = false;
					gameState.characterDead = false;
					gameState.currentLevel = LEVEL_4;
					gameState.currentScore = 0;
					gameState.damageTaken = 0;
					resetCharacterToDefault();
					for (int i = 0; i < MAX_ENEMIES; i++) {
						enemies[i].active = false;
					}
					images.gameBackGround = iLoadImage("Images/4_game_background.png");
				}
				// ------------------------------------------
			}
		}
		// ------------------------------------------
		else if (gameState.showInstructions || gameState.showAbout || gameState.gameStarted || gameState.showHighScores)
		{
			if (isPointInButton(mx, my, ui.returnButton))
			{
				if (gameState.gameStarted && (gameState.characterDead || (gameState.levelComplete && (gameState.currentLevel == LEVEL_3 || gameState.currentLevel == LEVEL_4)))) {
					saveScore(gameState.currentScore);
				}
				// ----------------------------------------------
				if (isGameMusicPlaying) {
					PlaySound(NULL, NULL, SND_PURGE);
					isGameMusicPlaying = false;
				}
				if (isBossMusicPlaying) {
					PlaySound(NULL, NULL, SND_PURGE);
					isBossMusicPlaying = false;
				}
				if (PlaySound("Music\\awesomeness.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
					isMenuMusicPlaying = true;
				}
				else {
					isMenuMusicPlaying = false;
				}
				gameState.showInstructions = false;
				gameState.showAbout = false;
				gameState.gameStarted = false;
				gameState.showHighScores = false;
				gameState.showMenu = true;
				gameState.characterDead = false;
				gameState.deathAnimationFinished = false;
				gameState.levelComplete = false;
				gameState.awaitingLevelProgression = false;
			}
		}
		if (gameState.gameStarted && isPointInButton(mx, my, ui.pauseButton)) {
			gameState.isPaused = !gameState.isPaused;
		}
	}
}
void iPassiveMouseMove(int mx, int my)
{
	ui.playButton.isHovered = false;
	ui.instructionsButton.isHovered = false;
	ui.aboutButton.isHovered = false;
	ui.exitButton.isHovered = false;
	ui.highScoresButton.isHovered = false;
	ui.levelButton.isHovered = false; // <<< ADDED FOR LEVEL JUMPING
	ui.returnButton.isHovered = false;
	ui.pauseButton.isHovered = false;
	// Initialize level buttons hover state to false
	ui.level1Button.isHovered = false;
	ui.level2Button.isHovered = false;
	ui.level3Button.isHovered = false;
	ui.level4Button.isHovered = false;
	if (gameState.showMenu)
	{
		ui.playButton.isHovered = isPointInButton(mx, my, ui.playButton);
		ui.instructionsButton.isHovered = isPointInButton(mx, my, ui.instructionsButton);
		ui.aboutButton.isHovered = isPointInButton(mx, my, ui.aboutButton);
		ui.highScoresButton.isHovered = isPointInButton(mx, my, ui.highScoresButton);
		ui.levelButton.isHovered = isPointInButton(mx, my, ui.levelButton); // <<< ADDED FOR LEVEL JUMPING
		ui.exitButton.isHovered = isPointInButton(mx, my, ui.exitButton);
	}
	// ------------------------------------------
	if (gameState.showInstructions || gameState.showAbout || gameState.gameStarted || gameState.showHighScores || gameState.showLevelSelect)
	{
		ui.returnButton.isHovered = isPointInButton(mx, my, ui.returnButton);
		// --- Add Hover Detection for Level Select Buttons ---
		if (gameState.showLevelSelect) {
			ui.level1Button.isHovered = isPointInButton(mx, my, ui.level1Button);
			ui.level2Button.isHovered = isPointInButton(mx, my, ui.level2Button);
			ui.level3Button.isHovered = isPointInButton(mx, my, ui.level3Button);
			ui.level4Button.isHovered = isPointInButton(mx, my, ui.level4Button);
		}
		// -----------------------------------------------------
	}
	// ------------------------------------------
	if (gameState.gameStarted)
	{
		ui.pauseButton.isHovered = isPointInButton(mx, my, ui.pauseButton);
	}
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                             Keyboard Handling
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void handleMenuNavigation(unsigned char key)
{
	if (key == 8)
	{
		if (gameState.showInstructions || gameState.showAbout || gameState.showHighScores || gameState.showLevelSelect)
		{
			gameState.showInstructions = false;
			gameState.showAbout = false;
			gameState.showHighScores = false;
			gameState.showLevelSelect = false; // <<< ADDED FOR LEVEL JUMPING
			gameState.showMenu = true;
		}
		else if (gameState.gameStarted)
		{
			if (isGameMusicPlaying) {
				PlaySound(NULL, NULL, SND_PURGE);
				isGameMusicPlaying = false;
			}
			if (PlaySound("Music\\awesomeness.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
				isMenuMusicPlaying = true;
			}
			else {
				isMenuMusicPlaying = false;
			}
			// -----------------------------------------
			gameState.gameStarted = false;
			gameState.showMenu = true;
			gameState.characterDead = false;
			gameState.deathAnimationFinished = false;
			gameState.levelComplete = false;
			gameState.awaitingLevelProgression = false;
		}
	}
	else if (key == 27)
	{
		if (gameState.gameStarted)
		{
			if (isGameMusicPlaying) {
				PlaySound(NULL, NULL, SND_PURGE);
				isGameMusicPlaying = false;
			}
			if (PlaySound("Music\\awesomeness.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
				isMenuMusicPlaying = true;
			}
			else {
				isMenuMusicPlaying = false;
			}
			// -----------------------------------------
			gameState.gameStarted = false;
			gameState.showMenu = true;
			gameState.characterDead = false;
			gameState.deathAnimationFinished = false;
			gameState.levelComplete = false;
			gameState.awaitingLevelProgression = false;
		}
		// Handle ESC from Level Select screen
		else if (gameState.showLevelSelect) // <<< ADDED FOR LEVEL JUMPING
		{
			gameState.showLevelSelect = false;
			gameState.showMenu = true;
		}
	}
	else if (key == ' ' && gameState.levelComplete && !gameState.characterDead && !gameState.awaitingLevelProgression) {
		gameState.awaitingLevelProgression = true;
		if (gameState.currentLevel == LEVEL_4) {
			gameState.currentScore += 3000;
			gameState.levelComplete = true;
			gameState.enemiesSpawned = false;
		}
		else if (gameState.currentLevel == LEVEL_3) {
			gameState.currentScore += 1000;
			gameState.currentLevel = LEVEL_4;
			character.health = CHARACTER_MAX_HEALTH;
			character.isDead = false;
			character.currentState = STATE_IDLE;
			character.x = 100;
			character.facingDirection = DIRECTION_RIGHT;
			gameState.enemiesSpawned = false;
			gameState.levelComplete = false;
			gameState.deathAnimationFinished = false;
			gameState.awaitingLevelProgression = false;
			images.gameBackGround = iLoadImage("Images/4_game_background.png");

			// Switch to boss music for level 4
			if (isGameMusicPlaying) {
				PlaySound(NULL, NULL, SND_PURGE);
				isGameMusicPlaying = false;
			}
			if (PlaySound("Music\\BOSS MUSIC.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
				isBossMusicPlaying = true;
			}
			else {
				isBossMusicPlaying = false;
			}
		}
		else {
			gameState.currentScore += 1000;
			gameState.currentLevel++;
			character.health = CHARACTER_MAX_HEALTH;
			character.isDead = false;
			character.currentState = STATE_IDLE;
			character.x = 100;
			character.facingDirection = DIRECTION_RIGHT;
			gameState.enemiesSpawned = false;
			gameState.levelComplete = false;
			gameState.deathAnimationFinished = false;
			gameState.awaitingLevelProgression = false;
			if (gameState.currentLevel == LEVEL_1) {
				images.gameBackGround = iLoadImage("Images/New Background.jpg");
			}
			else if (gameState.currentLevel == LEVEL_2) {
				images.gameBackGround = iLoadImage("Images/dead forest.png");
			}
			else if (gameState.currentLevel == LEVEL_3) {
				images.gameBackGround = iLoadImage("Images/2_game_background.png");
			}
			else if (gameState.currentLevel == LEVEL_4) {
				images.gameBackGround = iLoadImage("Images/4_game_background.png");
			}
		}
		if ((gameState.currentLevel == LEVEL_3 || gameState.currentLevel == LEVEL_4) && !gameState.awaitingLevelProgression) {
			gameState.awaitingLevelProgression = false;
		}
	}
	else if (key == 'p' || key == 'P') {
		if (gameState.gameStarted && !gameState.characterDead && !gameState.levelComplete) {
			gameState.isPaused = !gameState.isPaused;
		}
	}
}
void handleGameControls(unsigned char key)
{
	if (gameState.isPaused) return;
	if (!inputState.keyPressed[key])
	{
		inputState.keyPressed[key] = true;
	}
	if (character.isAttacking || character.isJumping || character.isDead) return;
	if (key == 'f' || key == 'F')
	{
		startAttack();
		return;
	}
	bool shiftPressed = (glutGetModifiers() & GLUT_ACTIVE_SHIFT);
	int currentSpeed = shiftPressed ? character.runSpeed : character.walkSpeed;
	if (key == 'a' || key == 'A')
	{
		if (character.x > 0)
		{
			character.x -= currentSpeed;
			character.facingDirection = DIRECTION_LEFT;
			if (shiftPressed)
			{
				character.currentState = STATE_RUNNING;
				character.isRunning = true;
			}
			else
			{
				character.currentState = STATE_WALKING;
				character.isRunning = false;
			}
			character.currentFrame = (character.currentFrame + 1) % 8;
		}
	}
	else if (key == 'd' || key == 'D')
	{
		if (character.x < WINDOW_WIDTH - character.width)
		{
			character.x += currentSpeed;
			character.facingDirection = DIRECTION_RIGHT;
			if (shiftPressed)
			{
				character.currentState = STATE_RUNNING;
				character.isRunning = true;
			}
			else
			{
				character.currentState = STATE_WALKING;
				character.isRunning = false;
			}
			character.currentFrame = (character.currentFrame + 1) % 8;
		}
	}
	else if (key == 'w' || key == 'W')
	{
		if (character.onGround && !character.isJumping)
		{
			character.isJumping = true;
			character.onGround = false;
			character.jumpVelocity = JUMP_STRENGTH;
			character.jumpFrame = 0;
			character.currentState = STATE_JUMPING;
		}
	}
}
void iKeyboard(unsigned char key)
{
	handleMenuNavigation(key);
	if (gameState.gameStarted)
	{
		handleGameControls(key);
	}
}
void iKeyboardUp(unsigned char key)
{
	if (gameState.gameStarted)
	{
		inputState.keyPressed[key] = false;
		if (key == 'a' || key == 'A' || key == 'd' || key == 'D')
		{
			if (!character.isJumping && !character.isAttacking && !character.isDead)
			{
				character.currentState = STATE_IDLE;
				character.currentFrame = 0;
				character.isRunning = false;
			}
		}
	}
}
void iSpecialKeyboard(unsigned char key) {}
void iSpecialKeyboardUp(unsigned char key) {}
void iMouseMove(int mx, int my) {}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                                 TIMER
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void gameTimer()
{
	if (gameState.gameStarted && !gameState.characterDead && !gameState.levelComplete && !gameState.isPaused)
	{
		if (!gameState.enemiesSpawned) {
			spawnEnemies();
		}
		updateCharacter();
		updateEnemies();
		updateEnemyAnimation();
	}
	else if (gameState.gameStarted && gameState.characterDead && !gameState.deathAnimationFinished) {
		updateCharacter();
	}
	else if (gameState.gameStarted && gameState.levelComplete && !gameState.characterDead) {
		updateCharacter();
		updateEnemies();
		updateEnemyAnimation();
	}
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                              INITIALIZATION
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void loadAllImages()
{
	// Load menu images
	images.titleImage = iLoadImage("Images/title.png");
	images.clickImage = iLoadImage("Images/click.png");
	images.backGround = iLoadImage("Images/bg.png");
	images.gameBackGround = iLoadImage("Images/New Background.jpg");
	images.playButtonImage = iLoadImage("Images/play.png");
	images.instructionsButtonImage = iLoadImage("Images/instructions.png");
	images.aboutButtonImage = iLoadImage("Images/about.png");
	images.exitButtonImage = iLoadImage("Images/exit.png");
	images.highScoresButtonImage = iLoadImage("Images/SCORES.png");
	images.levelButtonImage = iLoadImage("Images/LEVEL.png"); // <<< ADDED FOR LEVEL JUMPING
	images.returnButtonImage = iLoadImage("Images/return1.png");
	images.pauseButtonImage = iLoadImage("Images/pngtree-vector-pause-icon-png-image_966642-removebg-preview.png");
	images.resumeButtonImage = iLoadImage("Images/images-removebg-preview.png");
	images.idleSprite[0] = iLoadImage("Images/Idle1.png");
	images.idleSprite[1] = iLoadImage("Images/Idle1L.png");
	// Walk sprites 
	images.walkSprites[0][0] = iLoadImage("Images/Walk1.png");
	images.walkSprites[0][1] = iLoadImage("Images/Walk2.png");
	images.walkSprites[0][2] = iLoadImage("Images/Walk3.png");
	images.walkSprites[0][3] = iLoadImage("Images/Walk4.png");
	images.walkSprites[0][4] = iLoadImage("Images/Walk5.png");
	images.walkSprites[0][5] = iLoadImage("Images/Walk6.png");
	images.walkSprites[0][6] = iLoadImage("Images/Walk7.png");
	images.walkSprites[0][7] = iLoadImage("Images/Walk8.png");
	images.walkSprites[1][0] = iLoadImage("Images/Walk1L.png");
	images.walkSprites[1][1] = iLoadImage("Images/Walk2L.png");
	images.walkSprites[1][2] = iLoadImage("Images/Walk3L.png");
	images.walkSprites[1][3] = iLoadImage("Images/Walk4L.png");
	images.walkSprites[1][4] = iLoadImage("Images/Walk5L.png");
	images.walkSprites[1][5] = iLoadImage("Images/Walk6L.png");
	images.walkSprites[1][6] = iLoadImage("Images/Walk7L.png");
	images.walkSprites[1][7] = iLoadImage("Images/Walk8L.png");
	// Run sprites
	images.runSprites[0][0] = iLoadImage("Images/Run1.png");
	images.runSprites[0][1] = iLoadImage("Images/Run2.png");
	images.runSprites[0][2] = iLoadImage("Images/Run3.png");
	images.runSprites[0][3] = iLoadImage("Images/Run4.png");
	images.runSprites[0][4] = iLoadImage("Images/Run5.png");
	images.runSprites[0][5] = iLoadImage("Images/Run6.png");
	images.runSprites[0][6] = iLoadImage("Images/Run7.png");
	images.runSprites[0][7] = iLoadImage("Images/Run8.png");
	images.runSprites[1][0] = iLoadImage("Images/Run1L.png");
	images.runSprites[1][1] = iLoadImage("Images/Run2L.png");
	images.runSprites[1][2] = iLoadImage("Images/Run3L.png");
	images.runSprites[1][3] = iLoadImage("Images/Run4L.png");
	images.runSprites[1][4] = iLoadImage("Images/Run5L.png");
	images.runSprites[1][5] = iLoadImage("Images/Run6L.png");
	images.runSprites[1][6] = iLoadImage("Images/Run7L.png");
	images.runSprites[1][7] = iLoadImage("Images/Run8L.png");
	// Jump sprites
	images.jumpSprites[0][0] = iLoadImage("Images/Jump1.png");
	images.jumpSprites[0][1] = iLoadImage("Images/Jump2.png");
	images.jumpSprites[0][2] = iLoadImage("Images/Jump3.png");
	images.jumpSprites[0][3] = iLoadImage("Images/Jump4.png");
	images.jumpSprites[0][4] = iLoadImage("Images/Jump5.png");
	images.jumpSprites[0][5] = iLoadImage("Images/Jump6.png");
	images.jumpSprites[0][6] = iLoadImage("Images/Jump7.png");
	images.jumpSprites[0][7] = iLoadImage("Images/Jump8.png");
	images.jumpSprites[0][8] = iLoadImage("Images/Jump9.png");
	images.jumpSprites[0][9] = iLoadImage("Images/Jump10.png");
	images.jumpSprites[0][10] = iLoadImage("Images/Jump11.png");
	images.jumpSprites[0][11] = iLoadImage("Images/Jump12.png");
	images.jumpSprites[1][0] = iLoadImage("Images/Jump1L.png");
	images.jumpSprites[1][1] = iLoadImage("Images/Jump2L.png");
	images.jumpSprites[1][2] = iLoadImage("Images/Jump3L.png");
	images.jumpSprites[1][3] = iLoadImage("Images/Jump4L.png");
	images.jumpSprites[1][4] = iLoadImage("Images/Jump5L.png");
	images.jumpSprites[1][5] = iLoadImage("Images/Jump6L.png");
	images.jumpSprites[1][6] = iLoadImage("Images/Jump7L.png");
	images.jumpSprites[1][7] = iLoadImage("Images/Jump8L.png");
	images.jumpSprites[1][8] = iLoadImage("Images/Jump9L.png");
	images.jumpSprites[1][9] = iLoadImage("Images/Jump10L.png");
	images.jumpSprites[1][10] = iLoadImage("Images/Jump11L.png");
	images.jumpSprites[1][11] = iLoadImage("Images/Jump12L.png");
	// Attack sprites
	images.attackSprites[0][0] = iLoadImage("Images/Attack1.png");
	images.attackSprites[0][1] = iLoadImage("Images/Attack2.png");
	images.attackSprites[0][2] = iLoadImage("Images/Attack3.png");
	images.attackSprites[0][3] = iLoadImage("Images/Attack4.png");
	images.attackSprites[0][4] = iLoadImage("Images/Attack5.png");
	images.attackSprites[1][0] = iLoadImage("Images/Attack1L.png");
	images.attackSprites[1][1] = iLoadImage("Images/Attack2L.png");
	images.attackSprites[1][2] = iLoadImage("Images/Attack3L.png");
	images.attackSprites[1][3] = iLoadImage("Images/Attack4L.png");
	images.attackSprites[1][4] = iLoadImage("Images/Attack5L.png");
	// Dying sprites 
	images.dyingSprite[0] = iLoadImage("Images/Dead1.png");
	images.dyingSprite[1] = iLoadImage("Images/Dead1L.png");
	// Death animation sprites 
	images.deathSprites[0][0] = iLoadImage("Images/Dead1.png");
	images.deathSprites[0][1] = iLoadImage("Images/Dead2.png");
	images.deathSprites[0][2] = iLoadImage("Images/Dead3.png");
	images.deathSprites[0][3] = iLoadImage("Images/Dead4.png");
	images.deathSprites[1][0] = iLoadImage("Images/Dead1L.png");
	images.deathSprites[1][1] = iLoadImage("Images/Dead2L.png");
	images.deathSprites[1][2] = iLoadImage("Images/Dead3L.png");
	images.deathSprites[1][3] = iLoadImage("Images/Dead4L.png");
	// Load enemy walking sprites - Level 1 
	images.enemyWalkSprites_L1[0] = iLoadImage("Images/0_Skeleton_Crusader_Walking_000L.png");
	images.enemyWalkSprites_L1[1] = iLoadImage("Images/0_Skeleton_Crusader_Walking_001L.png");
	images.enemyWalkSprites_L1[2] = iLoadImage("Images/0_Skeleton_Crusader_Walking_002L.png");
	images.enemyWalkSprites_L1[3] = iLoadImage("Images/0_Skeleton_Crusader_Walking_003L.png");
	images.enemyWalkSprites_L1[4] = iLoadImage("Images/0_Skeleton_Crusader_Walking_004L.png");
	images.enemyWalkSprites_L1[5] = iLoadImage("Images/0_Skeleton_Crusader_Walking_005L.png");
	images.enemyWalkSprites_L1[6] = iLoadImage("Images/0_Skeleton_Crusader_Walking_006L.png");
	images.enemyWalkSprites_L1[7] = iLoadImage("Images/0_Skeleton_Crusader_Walking_007L.png");
	images.enemyWalkSprites_L1[8] = iLoadImage("Images/0_Skeleton_Crusader_Walking_008L.png");
	images.enemyWalkSprites_L1[9] = iLoadImage("Images/0_Skeleton_Crusader_Walking_009L.png");
	images.enemyWalkSprites_L1[10] = iLoadImage("Images/0_Skeleton_Crusader_Walking_010L.png");
	images.enemyWalkSprites_L1[11] = iLoadImage("Images/0_Skeleton_Crusader_Walking_011L.png");
	images.enemyWalkSprites_L1[12] = iLoadImage("Images/0_Skeleton_Crusader_Walking_012L.png");
	images.enemyWalkSprites_L1[13] = iLoadImage("Images/0_Skeleton_Crusader_Walking_013L.png");
	images.enemyWalkSprites_L1[14] = iLoadImage("Images/0_Skeleton_Crusader_Walking_014L.png");
	images.enemyWalkSprites_L1[15] = iLoadImage("Images/0_Skeleton_Crusader_Walking_015L.png");
	images.enemyWalkSprites_L1[16] = iLoadImage("Images/0_Skeleton_Crusader_Walking_016L.png");
	images.enemyWalkSprites_L1[17] = iLoadImage("Images/0_Skeleton_Crusader_Walking_017L.png");
	images.enemyWalkSprites_L1[18] = iLoadImage("Images/0_Skeleton_Crusader_Walking_018L.png");
	images.enemyWalkSprites_L1[19] = iLoadImage("Images/0_Skeleton_Crusader_Walking_019L.png");
	images.enemyWalkSprites_L1[20] = iLoadImage("Images/0_Skeleton_Crusader_Walking_020L.png");
	images.enemyWalkSprites_L1[21] = iLoadImage("Images/0_Skeleton_Crusader_Walking_021L.png");
	images.enemyWalkSprites_L1[22] = iLoadImage("Images/0_Skeleton_Crusader_Walking_022L.png");
	images.enemyWalkSprites_L1[23] = iLoadImage("Images/0_Skeleton_Crusader_Walking_023L.png");
	// Load enemy walking sprites - Level 1 
	images.enemyWalkSpritesRight_L1[0] = iLoadImage("Images/0_Skeleton_Crusader_Walking_000.png");
	images.enemyWalkSpritesRight_L1[1] = iLoadImage("Images/0_Skeleton_Crusader_Walking_001.png");
	images.enemyWalkSpritesRight_L1[2] = iLoadImage("Images/0_Skeleton_Crusader_Walking_002.png");
	images.enemyWalkSpritesRight_L1[3] = iLoadImage("Images/0_Skeleton_Crusader_Walking_003.png");
	images.enemyWalkSpritesRight_L1[4] = iLoadImage("Images/0_Skeleton_Crusader_Walking_004.png");
	images.enemyWalkSpritesRight_L1[5] = iLoadImage("Images/0_Skeleton_Crusader_Walking_005.png");
	images.enemyWalkSpritesRight_L1[6] = iLoadImage("Images/0_Skeleton_Crusader_Walking_006.png");
	images.enemyWalkSpritesRight_L1[7] = iLoadImage("Images/0_Skeleton_Crusader_Walking_007.png");
	images.enemyWalkSpritesRight_L1[8] = iLoadImage("Images/0_Skeleton_Crusader_Walking_008.png");
	images.enemyWalkSpritesRight_L1[9] = iLoadImage("Images/0_Skeleton_Crusader_Walking_009.png");
	images.enemyWalkSpritesRight_L1[10] = iLoadImage("Images/0_Skeleton_Crusader_Walking_010.png");
	images.enemyWalkSpritesRight_L1[11] = iLoadImage("Images/0_Skeleton_Crusader_Walking_011.png");
	images.enemyWalkSpritesRight_L1[12] = iLoadImage("Images/0_Skeleton_Crusader_Walking_012.png");
	images.enemyWalkSpritesRight_L1[13] = iLoadImage("Images/0_Skeleton_Crusader_Walking_013.png");
	images.enemyWalkSpritesRight_L1[14] = iLoadImage("Images/0_Skeleton_Crusader_Walking_014.png");
	images.enemyWalkSpritesRight_L1[15] = iLoadImage("Images/0_Skeleton_Crusader_Walking_015.png");
	images.enemyWalkSpritesRight_L1[16] = iLoadImage("Images/0_Skeleton_Crusader_Walking_016.png");
	images.enemyWalkSpritesRight_L1[17] = iLoadImage("Images/0_Skeleton_Crusader_Walking_017.png");
	images.enemyWalkSpritesRight_L1[18] = iLoadImage("Images/0_Skeleton_Crusader_Walking_018.png");
	images.enemyWalkSpritesRight_L1[19] = iLoadImage("Images/0_Skeleton_Crusader_Walking_019.png");
	images.enemyWalkSpritesRight_L1[20] = iLoadImage("Images/0_Skeleton_Crusader_Walking_020.png");
	images.enemyWalkSpritesRight_L1[21] = iLoadImage("Images/0_Skeleton_Crusader_Walking_021.png");
	images.enemyWalkSpritesRight_L1[22] = iLoadImage("Images/0_Skeleton_Crusader_Walking_022.png");
	images.enemyWalkSpritesRight_L1[23] = iLoadImage("Images/0_Skeleton_Crusader_Walking_023.png");
	// Load enemy stunned sprite - Level 1 
	images.enemyStunnedSprite_L1 = iLoadImage("Images/0_Skeleton_Crusader_Idle_000L.png");
	// Load enemy dying sprites - Level 1 
	images.enemyDyingSprites_L1[0] = iLoadImage("Images/0_Skeleton_Crusader_Dying_000L.png");
	images.enemyDyingSprites_L1[1] = iLoadImage("Images/0_Skeleton_Crusader_Dying_001L.png");
	images.enemyDyingSprites_L1[2] = iLoadImage("Images/0_Skeleton_Crusader_Dying_002L.png");
	images.enemyDyingSprites_L1[3] = iLoadImage("Images/0_Skeleton_Crusader_Dying_003L.png");
	images.enemyDyingSprites_L1[4] = iLoadImage("Images/0_Skeleton_Crusader_Dying_004L.png");
	images.enemyDyingSprites_L1[5] = iLoadImage("Images/0_Skeleton_Crusader_Dying_005L.png");
	images.enemyDyingSprites_L1[6] = iLoadImage("Images/0_Skeleton_Crusader_Dying_006L.png");
	images.enemyDyingSprites_L1[7] = iLoadImage("Images/0_Skeleton_Crusader_Dying_007L.png");
	images.enemyDyingSprites_L1[8] = iLoadImage("Images/0_Skeleton_Crusader_Dying_008L.png");
	images.enemyDyingSprites_L1[9] = iLoadImage("Images/0_Skeleton_Crusader_Dying_009L.png");
	images.enemyDyingSprites_L1[10] = iLoadImage("Images/0_Skeleton_Crusader_Dying_010L.png");
	images.enemyDyingSprites_L1[11] = iLoadImage("Images/0_Skeleton_Crusader_Dying_011L.png");
	images.enemyDyingSprites_L1[12] = iLoadImage("Images/0_Skeleton_Crusader_Dying_012L.png");
	images.enemyDyingSprites_L1[13] = iLoadImage("Images/0_Skeleton_Crusader_Dying_013L.png");
	images.enemyDyingSprites_L1[14] = iLoadImage("Images/0_Skeleton_Crusader_Dying_014L.png");
	// Load enemy attack sprites - Level 1 
	images.enemyAttackSprites_L1[0] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_000L.png");
	images.enemyAttackSprites_L1[1] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_001L.png");
	images.enemyAttackSprites_L1[2] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_002L.png");
	images.enemyAttackSprites_L1[3] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_003L.png");
	images.enemyAttackSprites_L1[4] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_004L.png");
	images.enemyAttackSprites_L1[5] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_005L.png");
	images.enemyAttackSprites_L1[6] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_006L.png");
	images.enemyAttackSprites_L1[7] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_007L.png");
	images.enemyAttackSprites_L1[8] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_008L.png");
	images.enemyAttackSprites_L1[9] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_009L.png");
	images.enemyAttackSprites_L1[10] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_010L.png");
	images.enemyAttackSprites_L1[11] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_011L.png");
	// Load enemy attack sprites - Level 1 
	images.enemyAttackSpritesRight_L1[0] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_000.png");
	images.enemyAttackSpritesRight_L1[1] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_001.png");
	images.enemyAttackSpritesRight_L1[2] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_002.png");
	images.enemyAttackSpritesRight_L1[3] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_003.png");
	images.enemyAttackSpritesRight_L1[4] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_004.png");
	images.enemyAttackSpritesRight_L1[5] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_005.png");
	images.enemyAttackSpritesRight_L1[6] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_006.png");
	images.enemyAttackSpritesRight_L1[7] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_007.png");
	images.enemyAttackSpritesRight_L1[8] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_008.png");
	images.enemyAttackSpritesRight_L1[9] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_009.png");
	images.enemyAttackSpritesRight_L1[10] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_010.png");
	images.enemyAttackSpritesRight_L1[11] = iLoadImage("Images/0_Skeleton_Crusader_Slashing_011.png");
	// Load enemy walking sprites - Level 2 
	images.enemyWalkSprites_L2[0] = iLoadImage("Images/Troll_02_1_WALK_000L.png");
	images.enemyWalkSprites_L2[1] = iLoadImage("Images/Troll_02_1_WALK_001L.png");
	images.enemyWalkSprites_L2[2] = iLoadImage("Images/Troll_02_1_WALK_002L.png");
	images.enemyWalkSprites_L2[3] = iLoadImage("Images/Troll_02_1_WALK_003L.png");
	images.enemyWalkSprites_L2[4] = iLoadImage("Images/Troll_02_1_WALK_004L.png");
	images.enemyWalkSprites_L2[5] = iLoadImage("Images/Troll_02_1_WALK_005L.png");
	images.enemyWalkSprites_L2[6] = iLoadImage("Images/Troll_02_1_WALK_006L.png");
	images.enemyWalkSprites_L2[7] = iLoadImage("Images/Troll_02_1_WALK_007L.png");
	images.enemyWalkSprites_L2[8] = iLoadImage("Images/Troll_02_1_WALK_008L.png");
	images.enemyWalkSprites_L2[9] = iLoadImage("Images/Troll_02_1_WALK_009L.png");
	// ------------------------------------------------------------
	// Load enemy walking sprites - Level 2 
	images.enemyWalkSpritesRight_L2[0] = iLoadImage("Images/Troll_02_1_WALK_000.png");
	images.enemyWalkSpritesRight_L2[1] = iLoadImage("Images/Troll_02_1_WALK_001.png");
	images.enemyWalkSpritesRight_L2[2] = iLoadImage("Images/Troll_02_1_WALK_002.png");
	images.enemyWalkSpritesRight_L2[3] = iLoadImage("Images/Troll_02_1_WALK_003.png");
	images.enemyWalkSpritesRight_L2[4] = iLoadImage("Images/Troll_02_1_WALK_004.png");
	images.enemyWalkSpritesRight_L2[5] = iLoadImage("Images/Troll_02_1_WALK_005.png");
	images.enemyWalkSpritesRight_L2[6] = iLoadImage("Images/Troll_02_1_WALK_006.png");
	images.enemyWalkSpritesRight_L2[7] = iLoadImage("Images/Troll_02_1_WALK_007.png");
	images.enemyWalkSpritesRight_L2[8] = iLoadImage("Images/Troll_02_1_WALK_008.png");
	images.enemyWalkSpritesRight_L2[9] = iLoadImage("Images/Troll_02_1_WALK_009.png");
	// -----------------------------------------------------------
	// Load enemy stunned sprite - Level 2 (Trolls)
	images.enemyStunnedSprite_L2_Left = iLoadImage("Images/Troll_02_1_IDLE_000L.png");
	images.enemyStunnedSprite_L2_Right = iLoadImage("Images/Troll_02_1_IDLE_000.png");
	// ---------------------------------------------
	// Load enemy dying sprites - Level 2 
	images.enemyDyingSprites_L2_Left[0] = iLoadImage("Images/Troll_02_1_DIE_000L.png");
	images.enemyDyingSprites_L2_Left[1] = iLoadImage("Images/Troll_02_1_DIE_001L.png");
	images.enemyDyingSprites_L2_Left[2] = iLoadImage("Images/Troll_02_1_DIE_002L.png");
	images.enemyDyingSprites_L2_Left[3] = iLoadImage("Images/Troll_02_1_DIE_003L.png");
	images.enemyDyingSprites_L2_Left[4] = iLoadImage("Images/Troll_02_1_DIE_004L.png");
	images.enemyDyingSprites_L2_Left[5] = iLoadImage("Images/Troll_02_1_DIE_005L.png");
	images.enemyDyingSprites_L2_Left[6] = iLoadImage("Images/Troll_02_1_DIE_006L.png");
	images.enemyDyingSprites_L2_Left[7] = iLoadImage("Images/Troll_02_1_DIE_007L.png");
	images.enemyDyingSprites_L2_Left[8] = iLoadImage("Images/Troll_02_1_DIE_008L.png");
	images.enemyDyingSprites_L2_Left[9] = iLoadImage("Images/Troll_02_1_DIE_009L.png");
	// ----------------------------------------------------------
	// Load enemy dying sprites - Level 2 
	images.enemyDyingSprites_L2_Right[0] = iLoadImage("Images/Troll_02_1_DIE_000.png");
	images.enemyDyingSprites_L2_Right[1] = iLoadImage("Images/Troll_02_1_DIE_001.png");
	images.enemyDyingSprites_L2_Right[2] = iLoadImage("Images/Troll_02_1_DIE_002.png");
	images.enemyDyingSprites_L2_Right[3] = iLoadImage("Images/Troll_02_1_DIE_003.png");
	images.enemyDyingSprites_L2_Right[4] = iLoadImage("Images/Troll_02_1_DIE_004.png");
	images.enemyDyingSprites_L2_Right[5] = iLoadImage("Images/Troll_02_1_DIE_005.png");
	images.enemyDyingSprites_L2_Right[6] = iLoadImage("Images/Troll_02_1_DIE_006.png");
	images.enemyDyingSprites_L2_Right[7] = iLoadImage("Images/Troll_02_1_DIE_007.png");
	images.enemyDyingSprites_L2_Right[8] = iLoadImage("Images/Troll_02_1_DIE_008.png");
	images.enemyDyingSprites_L2_Right[9] = iLoadImage("Images/Troll_02_1_DIE_009.png");
	// ---------------------------------------------------------
	// Load enemy attack sprites - Level 2 
	images.enemyAttackSprites_L2[0] = iLoadImage("Images/Troll_02_1_ATTACK_000L.png");
	images.enemyAttackSprites_L2[1] = iLoadImage("Images/Troll_02_1_ATTACK_001L.png");
	images.enemyAttackSprites_L2[2] = iLoadImage("Images/Troll_02_1_ATTACK_002L.png");
	images.enemyAttackSprites_L2[3] = iLoadImage("Images/Troll_02_1_ATTACK_003L.png");
	images.enemyAttackSprites_L2[4] = iLoadImage("Images/Troll_02_1_ATTACK_004L.png");
	images.enemyAttackSprites_L2[5] = iLoadImage("Images/Troll_02_1_ATTACK_005L.png");
	images.enemyAttackSprites_L2[6] = iLoadImage("Images/Troll_02_1_ATTACK_006L.png");
	images.enemyAttackSprites_L2[7] = iLoadImage("Images/Troll_02_1_ATTACK_007L.png");
	images.enemyAttackSprites_L2[8] = iLoadImage("Images/Troll_02_1_ATTACK_008L.png");
	images.enemyAttackSprites_L2[9] = iLoadImage("Images/Troll_02_1_ATTACK_009L.png");
	// ----------------------------------------------------------
	// Load enemy attack sprites - Level 2 
	images.enemyAttackSpritesRight_L2[0] = iLoadImage("Images/Troll_02_1_ATTACK_000.png");
	images.enemyAttackSpritesRight_L2[1] = iLoadImage("Images/Troll_02_1_ATTACK_001.png");
	images.enemyAttackSpritesRight_L2[2] = iLoadImage("Images/Troll_02_1_ATTACK_002.png");
	images.enemyAttackSpritesRight_L2[3] = iLoadImage("Images/Troll_02_1_ATTACK_003.png");
	images.enemyAttackSpritesRight_L2[4] = iLoadImage("Images/Troll_02_1_ATTACK_004.png");
	images.enemyAttackSpritesRight_L2[5] = iLoadImage("Images/Troll_02_1_ATTACK_005.png");
	images.enemyAttackSpritesRight_L2[6] = iLoadImage("Images/Troll_02_1_ATTACK_006.png");
	images.enemyAttackSpritesRight_L2[7] = iLoadImage("Images/Troll_02_1_ATTACK_007.png");
	images.enemyAttackSpritesRight_L2[8] = iLoadImage("Images/Troll_02_1_ATTACK_008.png");
	images.enemyAttackSpritesRight_L2[9] = iLoadImage("Images/Troll_02_1_ATTACK_009.png");
	// ---------------------------------------------------------
	// --- Load Level 3 (Predator) Sprites ---
	// Load enemy idle sprite - Level 3 
	images.enemyIdleSprite_L3_Left = iLoadImage("Images/predatormask__0000_idle_1L.png");
	images.enemyIdleSprite_L3_Right = iLoadImage("Images/predatormask__0000_idle_1.png");
	// Load enemy walking sprites - Level 3 
	images.enemyWalkSprites_L3[0] = iLoadImage("Images/predatormask__0003_turn_1L.png");
	images.enemyWalkSprites_L3[1] = iLoadImage("Images/predatormask__0003_turn_2L.png");
	images.enemyWalkSprites_L3[2] = iLoadImage("Images/predatormask__0003_turn_3L.png");
	images.enemyWalkSprites_L3[3] = iLoadImage("Images/predatormask__0006_walk_1L.png");
	images.enemyWalkSprites_L3[4] = iLoadImage("Images/predatormask__0006_walk_2L.png");
	images.enemyWalkSprites_L3[5] = iLoadImage("Images/predatormask__0006_walk_3L.png");
	images.enemyWalkSprites_L3[6] = iLoadImage("Images/predatormask__0006_walk_4L.png");
	images.enemyWalkSprites_L3[7] = iLoadImage("Images/predatormask__0006_walk_5L.png");
	images.enemyWalkSprites_L3[8] = iLoadImage("Images/predatormask__0006_walk_6L.png");
	// Load enemy walking sprites - Level 3 
	images.enemyWalkSpritesRight_L3[0] = iLoadImage("Images/predatormask__0003_turn_1.png");
	images.enemyWalkSpritesRight_L3[1] = iLoadImage("Images/predatormask__0003_turn_2.png");
	images.enemyWalkSpritesRight_L3[2] = iLoadImage("Images/predatormask__0003_turn_3.png");
	images.enemyWalkSpritesRight_L3[3] = iLoadImage("Images/predatormask__0006_walk_1.png");
	images.enemyWalkSpritesRight_L3[4] = iLoadImage("Images/predatormask__0006_walk_2.png");
	images.enemyWalkSpritesRight_L3[5] = iLoadImage("Images/predatormask__0006_walk_3.png");
	images.enemyWalkSpritesRight_L3[6] = iLoadImage("Images/predatormask__0006_walk_4.png");
	images.enemyWalkSpritesRight_L3[7] = iLoadImage("Images/predatormask__0006_walk_5.png");
	images.enemyWalkSpritesRight_L3[8] = iLoadImage("Images/predatormask__0006_walk_6.png");
	// Load enemy death sprites - Level 3 
	images.enemyDeathSprites_L3_Left[0] = iLoadImage("Images/predatormask__0022_dead_1L.png");
	images.enemyDeathSprites_L3_Left[1] = iLoadImage("Images/predatormask__0022_dead_2L.png");
	images.enemyDeathSprites_L3_Left[2] = iLoadImage("Images/predatormask__0022_dead_3L.png");
	images.enemyDeathSprites_L3_Left[3] = iLoadImage("Images/predatormask__0022_dead_4L.png");
	images.enemyDeathSprites_L3_Left[4] = iLoadImage("Images/predatormask__0022_dead_5L.png");
	// Load enemy death sprites - Level 3 
	images.enemyDeathSprites_L3_Right[0] = iLoadImage("Images/predatormask__0022_dead_1.png");
	images.enemyDeathSprites_L3_Right[1] = iLoadImage("Images/predatormask__0022_dead_2.png");
	images.enemyDeathSprites_L3_Right[2] = iLoadImage("Images/predatormask__0022_dead_3.png");
	images.enemyDeathSprites_L3_Right[3] = iLoadImage("Images/predatormask__0022_dead_4.png");
	images.enemyDeathSprites_L3_Right[4] = iLoadImage("Images/predatormask__0022_dead_5.png");
	// Load enemy attack sprites - Level 3 
	images.enemyAttackSprites_L3[0] = iLoadImage("Images/predatormask__0031_attack_1L.png");
	images.enemyAttackSprites_L3[1] = iLoadImage("Images/predatormask__0031_attack_2L.png");
	images.enemyAttackSprites_L3[2] = iLoadImage("Images/predatormask__0031_attack_3L.png");
	images.enemyAttackSprites_L3[3] = iLoadImage("Images/predatormask__0031_attack_4L.png");
	// Load enemy attack sprites - Level 3 
	images.enemyAttackSpritesRight_L3[0] = iLoadImage("Images/predatormask__0031_attack_1.png");
	images.enemyAttackSpritesRight_L3[1] = iLoadImage("Images/predatormask__0031_attack_2.png");
	images.enemyAttackSpritesRight_L3[2] = iLoadImage("Images/predatormask__0031_attack_3.png");
	images.enemyAttackSpritesRight_L3[3] = iLoadImage("Images/predatormask__0031_attack_4.png");
	// --- Load Level 4 (Demon Boss) Sprites ---
	images.enemyWalkSprites_L4[0] = iLoadImage("Images/demon_walk_1.png");
	images.enemyWalkSprites_L4[1] = iLoadImage("Images/demon_walk_2.png");
	images.enemyWalkSprites_L4[2] = iLoadImage("Images/demon_walk_3.png");
	images.enemyWalkSprites_L4[3] = iLoadImage("Images/demon_walk_4.png");
	images.enemyWalkSprites_L4[4] = iLoadImage("Images/demon_walk_5.png");
	images.enemyWalkSprites_L4[5] = iLoadImage("Images/demon_walk_6.png");
	images.enemyWalkSprites_L4[6] = iLoadImage("Images/demon_walk_7.png");
	images.enemyWalkSprites_L4[7] = iLoadImage("Images/demon_walk_8.png");
	images.enemyWalkSprites_L4[8] = iLoadImage("Images/demon_walk_9.png");
	images.enemyWalkSprites_L4[9] = iLoadImage("Images/demon_walk_10.png");
	images.enemyWalkSprites_L4[10] = iLoadImage("Images/demon_walk_11.png");
	images.enemyWalkSprites_L4[11] = iLoadImage("Images/demon_walk_12.png");
	// Load mirrored versions for right-facing
	images.enemyWalkSpritesRight_L4[0] = iLoadImage("Images/demon_walk_1L.png"); // Assuming no separate right sprites, using same
	images.enemyWalkSpritesRight_L4[1] = iLoadImage("Images/demon_walk_2L.png");
	images.enemyWalkSpritesRight_L4[2] = iLoadImage("Images/demon_walk_3L.png");
	images.enemyWalkSpritesRight_L4[3] = iLoadImage("Images/demon_walk_4L.png");
	images.enemyWalkSpritesRight_L4[4] = iLoadImage("Images/demon_walk_5L.png");
	images.enemyWalkSpritesRight_L4[5] = iLoadImage("Images/demon_walk_6L.png");
	images.enemyWalkSpritesRight_L4[6] = iLoadImage("Images/demon_walk_7L.png");
	images.enemyWalkSpritesRight_L4[7] = iLoadImage("Images/demon_walk_8L.png");
	images.enemyWalkSpritesRight_L4[8] = iLoadImage("Images/demon_walk_9L.png");
	images.enemyWalkSpritesRight_L4[9] = iLoadImage("Images/demon_walk_10L.png");
	images.enemyWalkSpritesRight_L4[10] = iLoadImage("Images/demon_walk_11L.png");
	images.enemyWalkSpritesRight_L4[11] = iLoadImage("Images/demon_walk_12L.png");

	// --- Load Level 4 (Demon Boss) Attack Sprites ---
	images.enemyAttackSprites_L4[0] = iLoadImage("Images/demon_cleave_1.png");
	images.enemyAttackSprites_L4[1] = iLoadImage("Images/demon_cleave_2.png");
	images.enemyAttackSprites_L4[2] = iLoadImage("Images/demon_cleave_3.png");
	images.enemyAttackSprites_L4[3] = iLoadImage("Images/demon_cleave_4.png");
	images.enemyAttackSprites_L4[4] = iLoadImage("Images/demon_cleave_5.png");
	images.enemyAttackSprites_L4[5] = iLoadImage("Images/demon_cleave_6.png");
	images.enemyAttackSprites_L4[6] = iLoadImage("Images/demon_cleave_7.png");
	images.enemyAttackSprites_L4[7] = iLoadImage("Images/demon_cleave_8.png");
	images.enemyAttackSprites_L4[8] = iLoadImage("Images/demon_cleave_9.png");
	images.enemyAttackSprites_L4[9] = iLoadImage("Images/demon_cleave_10.png");
	images.enemyAttackSprites_L4[10] = iLoadImage("Images/demon_cleave_11.png");
	images.enemyAttackSprites_L4[11] = iLoadImage("Images/demon_cleave_12.png");
	images.enemyAttackSprites_L4[12] = iLoadImage("Images/demon_cleave_13.png");
	images.enemyAttackSprites_L4[13] = iLoadImage("Images/demon_cleave_14.png");
	images.enemyAttackSprites_L4[14] = iLoadImage("Images/demon_cleave_15.png");

	// Load mirrored versions for right-facing attack
	images.enemyAttackSpritesRight_L4[0] = iLoadImage("Images/demon_cleave_1L.png");
	images.enemyAttackSpritesRight_L4[1] = iLoadImage("Images/demon_cleave_2L.png");
	images.enemyAttackSpritesRight_L4[2] = iLoadImage("Images/demon_cleave_3L.png");
	images.enemyAttackSpritesRight_L4[3] = iLoadImage("Images/demon_cleave_4L.png");
	images.enemyAttackSpritesRight_L4[4] = iLoadImage("Images/demon_cleave_5L.png");
	images.enemyAttackSpritesRight_L4[5] = iLoadImage("Images/demon_cleave_6L.png");
	images.enemyAttackSpritesRight_L4[6] = iLoadImage("Images/demon_cleave_7L.png");
	images.enemyAttackSpritesRight_L4[7] = iLoadImage("Images/demon_cleave_8L.png");
	images.enemyAttackSpritesRight_L4[8] = iLoadImage("Images/demon_cleave_9L.png");
	images.enemyAttackSpritesRight_L4[9] = iLoadImage("Images/demon_cleave_10L.png");
	images.enemyAttackSpritesRight_L4[10] = iLoadImage("Images/demon_cleave_11L.png");
	images.enemyAttackSpritesRight_L4[11] = iLoadImage("Images/demon_cleave_12L.png");
	images.enemyAttackSpritesRight_L4[12] = iLoadImage("Images/demon_cleave_13L.png");
	images.enemyAttackSpritesRight_L4[13] = iLoadImage("Images/demon_cleave_14L.png");
	images.enemyAttackSpritesRight_L4[14] = iLoadImage("Images/demon_cleave_15L.png");

	// --- Load Level 4 (Demon Boss) Death Sprites ---
	images.enemyDyingSprites_L4[0] = iLoadImage("Images/demon_death_1.png");
	images.enemyDyingSprites_L4[1] = iLoadImage("Images/demon_death_2.png");
	images.enemyDyingSprites_L4[2] = iLoadImage("Images/demon_death_3.png");
	images.enemyDyingSprites_L4[3] = iLoadImage("Images/demon_death_4.png");
	images.enemyDyingSprites_L4[4] = iLoadImage("Images/demon_death_5.png");
	images.enemyDyingSprites_L4[5] = iLoadImage("Images/demon_death_6.png");
	images.enemyDyingSprites_L4[6] = iLoadImage("Images/demon_death_7.png");
	images.enemyDyingSprites_L4[7] = iLoadImage("Images/demon_death_8.png");
	images.enemyDyingSprites_L4[8] = iLoadImage("Images/demon_death_9.png");
	images.enemyDyingSprites_L4[9] = iLoadImage("Images/demon_death_10.png");
	images.enemyDyingSprites_L4[10] = iLoadImage("Images/demon_death_11.png");
	images.enemyDyingSprites_L4[11] = iLoadImage("Images/demon_death_12.png");
	images.enemyDyingSprites_L4[12] = iLoadImage("Images/demon_death_13.png");
	images.enemyDyingSprites_L4[13] = iLoadImage("Images/demon_death_14.png");
	images.enemyDyingSprites_L4[14] = iLoadImage("Images/demon_death_15.png");
	images.enemyDyingSprites_L4[15] = iLoadImage("Images/demon_death_16.png");
	images.enemyDyingSprites_L4[16] = iLoadImage("Images/demon_death_17.png");
	images.enemyDyingSprites_L4[17] = iLoadImage("Images/demon_death_18.png");
	images.enemyDyingSprites_L4[18] = iLoadImage("Images/demon_death_19.png");
	images.enemyDyingSprites_L4[19] = iLoadImage("Images/demon_death_20.png");
	images.enemyDyingSprites_L4[20] = iLoadImage("Images/demon_death_21.png");
	images.enemyDyingSprites_L4[21] = iLoadImage("Images/demon_death_22.png");

	// Load mirrored versions for right-facing death
	images.enemyDyingSpritesRight_L4[0] = iLoadImage("Images/demon_death_1L.png");
	images.enemyDyingSpritesRight_L4[1] = iLoadImage("Images/demon_death_2L.png");
	images.enemyDyingSpritesRight_L4[2] = iLoadImage("Images/demon_death_3L.png");
	images.enemyDyingSpritesRight_L4[3] = iLoadImage("Images/demon_death_4L.png");
	images.enemyDyingSpritesRight_L4[4] = iLoadImage("Images/demon_death_5L.png");
	images.enemyDyingSpritesRight_L4[5] = iLoadImage("Images/demon_death_6L.png");
	images.enemyDyingSpritesRight_L4[6] = iLoadImage("Images/demon_death_7L.png");
	images.enemyDyingSpritesRight_L4[7] = iLoadImage("Images/demon_death_8L.png");
	images.enemyDyingSpritesRight_L4[8] = iLoadImage("Images/demon_death_9L.png");
	images.enemyDyingSpritesRight_L4[9] = iLoadImage("Images/demon_death_10L.png");
	images.enemyDyingSpritesRight_L4[10] = iLoadImage("Images/demon_death_11L.png");
	images.enemyDyingSpritesRight_L4[11] = iLoadImage("Images/demon_death_12L.png");
	images.enemyDyingSpritesRight_L4[12] = iLoadImage("Images/demon_death_13L.png");
	images.enemyDyingSpritesRight_L4[13] = iLoadImage("Images/demon_death_14L.png");
	images.enemyDyingSpritesRight_L4[14] = iLoadImage("Images/demon_death_15L.png");
	images.enemyDyingSpritesRight_L4[15] = iLoadImage("Images/demon_death_16L.png");
	images.enemyDyingSpritesRight_L4[16] = iLoadImage("Images/demon_death_17L.png");
	images.enemyDyingSpritesRight_L4[17] = iLoadImage("Images/demon_death_18L.png");
	images.enemyDyingSpritesRight_L4[18] = iLoadImage("Images/demon_death_19L.png");
	images.enemyDyingSpritesRight_L4[19] = iLoadImage("Images/demon_death_20L.png");
	images.enemyDyingSpritesRight_L4[20] = iLoadImage("Images/demon_death_21L.png");
	images.enemyDyingSpritesRight_L4[21] = iLoadImage("Images/demon_death_22L.png");

	// --- Load Level Selection Button Images ---
	images.level1ButtonImage = iLoadImage("Images/LEVEL 1.png");
	images.level2ButtonImage = iLoadImage("Images/LEVEL 2.png");
	images.level3ButtonImage = iLoadImage("Images/LEVEL 3.png");
	images.level4ButtonImage = iLoadImage("Images/LEVEL 4.png");
	// -----------------------------------------
}
void initializeGameState()
{
	gameState.showMenu = true;
	gameState.showInstructions = false;
	gameState.showAbout = false;
	gameState.showHighScores = false;
	gameState.showLevelSelect = false; // <<< ADDED FOR LEVEL JUMPING
	gameState.gameStarted = false;
	gameState.enemiesSpawned = false;
	gameState.enemiesKilled = 0;
	gameState.nextEnemyToSpawn = 0;
	gameState.characterDead = false;
	gameState.characterHealth = CHARACTER_MAX_HEALTH;
	gameState.currentLevel = LEVEL_1;
	gameState.enemiesSpawnedInLevel = 0;
	gameState.levelComplete = false;
	gameState.awaitingLevelProgression = false;
	gameState.deathAnimationFinished = false;
	gameState.isPaused = false;
	gameState.currentScore = 0;
	gameState.damageTaken = 0;
}
void initializeCharacter()
{
	character.x = 100;
	character.y = GROUND_Y;
	character.width = CHARACTER_WIDTH;
	character.height = CHARACTER_HEIGHT;
	character.walkSpeed = WALK_SPEED;
	character.runSpeed = RUN_SPEED;
	character.facingDirection = DIRECTION_RIGHT;
	character.moveLeft = false;
	character.moveRight = false;
	character.isRunning = false;
	character.isJumping = false;
	character.isAttacking = false;
	character.onGround = true;
	character.isDead = false;
	character.currentState = STATE_IDLE;
	character.currentFrame = 0;
	character.animationCounter = 0;
	character.animationSpeed = 5;
	character.attackFrame = 0;
	character.attackAnimationCounter = 0;
	character.attackAnimationSpeed = 3;
	character.jumpVelocity = 0;
	character.jumpFrame = 0;
	character.health = CHARACTER_MAX_HEALTH;
	character.collisionCooldown = false;
	character.collisionTimer = 0;
	character.deathFrame = 0;
	character.deathAnimationCounter = 0;
	character.deathAnimationSpeed = 5;
}
void initializeEnemies()
{
	for (int i = 0; i < MAX_ENEMIES; i++) {
		enemies[i].active = false;
		enemies[i].x = 0;
		enemies[i].y = GROUND_Y;
		enemies[i].width = ENEMY_WIDTH;
		enemies[i].height = ENEMY_HEIGHT;
		enemies[i].currentFrame = 0;
		enemies[i].dyingFrame = 0;
		enemies[i].animationCounter = 0;
		enemies[i].animationSpeed = 5;
		enemies[i].currentState = ENEMY_WALKING;
		enemies[i].facingDirection = ENEMY_DIRECTION_LEFT;
		enemies[i].type = ENEMY_TYPE_SKELETON;
		enemies[i].hitCount = 0;
		enemies[i].alreadyHit = false;
		enemies[i].hasBeenDodged = false;
		enemies[i].health = ENEMY_MAX_HEALTH;
		enemies[i].collisionCooldown = false;
		enemies[i].collisionTimer = 0;
		enemies[i].isAttacking = false;
		enemies[i].attackFrame = 0;
		enemies[i].attackAnimationCounter = 0;
		enemies[i].attackAnimationSpeed = 1;
		enemies[i].attackCooldownCounter = 0;
		enemies[i].stunTimer = 0;
		// ------------------------------------------
	}
}
void initializeUI()
{
	ui.playButton.x = 100;
	ui.playButton.y = 360;
	ui.playButton.width = 200;
	ui.playButton.height = 60;
	ui.playButton.isHovered = false;
	ui.instructionsButton.x = 100;
	ui.instructionsButton.y = 290;
	ui.instructionsButton.width = 200;
	ui.instructionsButton.height = 60;
	ui.instructionsButton.isHovered = false;
	ui.aboutButton.x = 100;
	ui.aboutButton.y = 220;
	ui.aboutButton.width = 200;
	ui.aboutButton.height = 60;
	ui.aboutButton.isHovered = false;
	// Initialize High Scores Button ---
	ui.highScoresButton.x = 100;
	ui.highScoresButton.y = 150;
	ui.highScoresButton.width = 200;
	ui.highScoresButton.height = 60;
	ui.highScoresButton.isHovered = false;
	// ------------------------------------------
	// Initialize Level Button ---
	ui.levelButton.x = 100;
	ui.levelButton.y = 80;
	ui.levelButton.width = 200;
	ui.levelButton.height = 60;
	ui.levelButton.isHovered = false;
	// ------------------------------------------
	ui.exitButton.x = 100;
	ui.exitButton.y = 10;
	ui.exitButton.width = 200;
	ui.exitButton.height = 60;
	ui.exitButton.isHovered = false;
	ui.clickButton.x = 400;
	ui.clickButton.y = 0;
	ui.clickButton.width = 0;
	ui.clickButton.height = 0;
	ui.clickButton.isHovered = false;
	ui.returnButton.x = 20;
	ui.returnButton.y = 530;
	ui.returnButton.width = 75;
	ui.returnButton.height = 75;
	ui.returnButton.isHovered = false;
	ui.pauseButton.x = WINDOW_WIDTH - 100;
	ui.pauseButton.y = 530;
	ui.pauseButton.width = 75;
	ui.pauseButton.height = 75;
	ui.pauseButton.isHovered = false;
	// --- Initialize Level Selection Buttons ---
	ui.level1Button.x = 300;
	ui.level1Button.y = 400;
	ui.level1Button.width = 200;
	ui.level1Button.height = 60;
	ui.level1Button.isHovered = false;
	ui.level2Button.x = 300;
	ui.level2Button.y = 320;
	ui.level2Button.width = 200;
	ui.level2Button.height = 60;
	ui.level2Button.isHovered = false;
	ui.level3Button.x = 300;
	ui.level3Button.y = 240;
	ui.level3Button.width = 200;
	ui.level3Button.height = 60;
	ui.level3Button.isHovered = false;
	ui.level4Button.x = 300;
	ui.level4Button.y = 160;
	ui.level4Button.width = 200;
	ui.level4Button.height = 60;
	ui.level4Button.isHovered = false;
	// ------------------------------------------
}
void initializeInputState()
{
	for (int i = 0; i < 256; i++)
	{
		inputState.keyPressed[i] = false;
		inputState.specialKeyPressed[i] = false;
	}
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//                                   MAIN
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
int main()
{
	srand(time(NULL));
	iInitialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Dawn Reborn");
	initializeGameState();
	initializeCharacter();
	initializeUI();
	initializeInputState();
	initializeEnemies();
	loadAllImages();
	if (PlaySound("Music\\awesomeness.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME)) {
		isMenuMusicPlaying = true;
	}
	else {
		isMenuMusicPlaying = false;
	}
	// -------------------------------
	iSetTimer(20, gameTimer);
	iStart();
	return 0;
}