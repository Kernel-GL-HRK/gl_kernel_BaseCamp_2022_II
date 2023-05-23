LIBS=controlling_servo.c scaning_ultrasonic.c out_on_display.c
MAIN=final_project.c
OUTPUT=FP


default: $(MAIN) $(LIBS)
	gcc $^ -lncurses -o $(OUTPUT).out 

run: default
	./$(OUTPUT).out

clean:
	rm *.out *.o

.PHONY: default run  