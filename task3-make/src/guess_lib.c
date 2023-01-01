#include "guess_lib.h"

const char WinMessage[] = "You win!";
const char LooseMessage[] = "You loose!";

int get_user_input(char *message, char *variants)
{
	char user_input = 0;
	printf("%s : ", message);
	scanf(" %c", &user_input);
	for (int i = 0; i < strlen(variants); i++) {
		if (user_input == variants[i]) {
			return i;
		}
	}
	puts("Wrong input");
	return -1;
}

int get_comp_input(void)
{
	srand(time(NULL));
	return (rand() % 10);
}

int guess_game(void)
{
	int user_input = -1;
	int comp_input = -1;

	do {
		user_input = get_user_input("Make your choise [0..9]: ",
					    "0123456789");
	} while (user_input < 0);

	printf(" Your choise is : %i\n", user_input);

	comp_input = get_comp_input();
	printf(" Comp choise is : %i\n", comp_input);

	if (user_input == comp_input) {
		puts(WinMessage);
		return 0;
	} else {
		puts(LooseMessage);
		return 1;
	}
	return -1;
}
