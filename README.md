## This STM32F4 embedded C program controls and interacts with various sensors and LEDs on a microcontroller board, providing real-time feedback through LEDs and USART communication. Below is an overview and breakdown of each component and its working.

### Code Overview

1. **Initialization of GPIO, ADC, Interrupt, and USART**:
   - **GPIO**: Configures General Purpose Input/Output (GPIO) pins for sensor inputs, LED outputs, and USART.
   - **ADC**: Sets up the Analog-to-Digital Converter (ADC) to read analog signals from the LDR sensor.
   - **Interrupt**: Configures an external interrupt on a button press to start the main system loop.
   - **USART**: Initializes USART2 for communication, allowing the system to send status messages to a connected terminal and receive commands.

2. **Main Program Execution**:
   - The program waits for a button press (external interrupt) to begin executing the main loop.
   - In the main loop, each sensor is checked, and based on their readings, the LEDs are activated accordingly. The program also sends sensor statuses to the terminal if they’re detected.
   - USART communication allows remote control, where specific characters (e.g., '1', '0') turn LEDs on and off.

### Function Breakdown

#### 1. `void init_gpio(void);`
   - Initializes GPIO pins for sensor inputs (PIR, ultrasonic, LDR, touch sensor) and LEDs.
   - Configures PA0 for button input with pull-up resistor and various pins for other sensors and LEDs.

#### 2. `void init_adc(void);`
   - Enables ADC1 for reading analog input on PB0 (connected to the LDR).
   - Configures ADC to start conversion on demand.

#### 3. `void configure_interrupt(void);`
   - Sets up an interrupt on PA0 (button), enabling it to trigger on a falling edge.
   - Maps EXTI0 (External Interrupt 0) to PA0 and enables it in the NVIC (Nested Vectored Interrupt Controller).

#### 4. `void EXTI0_IRQHandler(void);`
   - Interrupt handler for EXTI0. Sets a flag (`start_system`) to begin the main loop upon button press and clears the interrupt pending bit.

#### 5. `void USART2_Init(void);`
   - Configures USART2 for serial communication with a baud rate of 9600.
   - Enables USART, receiver, and transmitter.

#### 6. `char USART2_Read(void);`
   - Waits for data to be received and returns the character received on USART2.

#### 7. `void USART2_Write(char c);`
   - Waits until USART2’s transmit buffer is empty and then sends a character.

#### 8. `void USART2_SendString(char *str);`
   - Sends a string over USART2 by sending each character one at a time.

### Main Program Loop

The main loop operates once the `start_system` flag is set (when the button is pressed).

1. **Sensor Checks**:
   - **PIR Sensor**: Reads PA11 to detect motion. If motion is detected, `pir_detected` is set to 1, turning on the associated LED.
   - **Ultrasonic Sensor**: Sends a trigger pulse on PA4 and checks for an echo on PA5. If an echo is detected, the associated LED turns on.
   - **LDR Sensor**: Reads analog data from PB0 and checks if the light level is below a set threshold. If yes, the LED for LDR is turned on.
   - **Touch Sensor**: Reads PA8 to detect a touch. If detected, it turns on an LED and sends a message via USART.

2. **USART Commands**:
   - Commands received via USART allow control over LEDs:
     - `'1'`: Turns on a specific LED.
     - `'0'`: Turns off that LED.
     - `'2'`: Sequentially turns all LEDs on, waits 2 seconds, then turns them off.
     - `'3'`: Executes a blinking sequence for LEDs, where each LED turns on and off in a specific order.

3. **LED Control**:
   - Based on sensor detections, LEDs are toggled on PA6, PA7, PA9, and PC13 (various statuses).
   - Messages are sent to a connected terminal via USART to indicate the sensor activation states.

### Key Concepts in the Code

- **Volatile Variables**: `adcData` and `start_system` are marked as `volatile`, which ensures they’re updated correctly and visible to all parts of the code, particularly in scenarios involving interrupts or external interactions.
- **Debounce Delay**: A small delay is added to avoid false triggering of the touch sensor.
- **USART for Debugging and Control**: The USART module enables the system to communicate with a terminal for debugging and remote control purposes, providing flexibility in sensor monitoring and LED manipulation.

### Practical Usage

This code is ideal for a basic security or monitoring system, utilizing sensors for motion, distance, light intensity, and touch. The USART interface allows a user to interact with the system remotely, making it suitable for embedded IoT applications where user feedback and control are needed.

### Team Members:
   - CB.EN.U4CSE22438 - Prasad Simhadri
   - CB.EN.U4CSE22439 - S. Siddhardh Reddy
   - CB.EN.U4CSE22458 - D. Vamsi Krishna
   - CB.EN.U4CSE22461 - B. Shanmuka Vardhan

     
Department of Computer Science and Engineering    
Amrita School of Computing, 
Amrita Vishwa Vidyapeetham, Coimbatore, India.
