libs=controlling_servo.c scaning_ultrasonic.c
main_file=final_project.c
output=FP


default: $(main_file) $(libs)
	gcc $^ -lncurses -o $(output) 

run: default
	./$(output)

.PHONY: default run  