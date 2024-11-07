#include "stm32f4xx.h"

// Function prototypes
void init_gpio(void);
void init_adc(void);
void configure_interrupt(void);
void EXTI0_IRQHandler(void); // Declare the interrupt handler
void USART2_Init(void);
char USART2_Read(void);
void USART2_Write(char c);
void USART2_SendString(char *str);

volatile int start_system = 0; // Flag to indicate when the system should start
volatile int adcData; // Make adcData volatile to ensure it's updated in the debugger
const int threshold = 1024;

void delay(int time) {
    for (int i = 0; i < time * 1000; i++);
}

int main(void) {
    init_gpio();       // Initialize GPIOs
    init_adc();        // Initialize ADC
    configure_interrupt(); // Configure external interrupt
    USART2_Init();     // Initialize USART2

    // Optionally, send an initial message to confirm setup
    USART2_SendString("USART Initialized\r\n");

    // Wait for the button press to start the system
    while (!start_system) {
        // Optionally, you can toggle an LED to indicate waiting state
        delay(100); // Delay for visibility
    }

    // Main loop
    while (1) {
        int pir_detected = 0;
        int ultrasonic_detected = 0;
        int ldr_detected = 0;

        // Check PIR Sensor on PA11
        if (GPIOA->IDR & (1U << 11)) { // If motion detected on PA11
            pir_detected = 1;
        }

        // Ultrasonic Sensor: Send trigger pulse
        GPIOA->ODR |= (1U << 4);  // Set PA4 high (Trigger)
        delay(10);                 // Short delay ~10 µs
        GPIOA->ODR &= ~(1U << 4); // Set PA4 low (Trigger)

        // Check for echo on PA5
        delay(1); // Adding a delay to wait for the echo response
        if (GPIOA->IDR & (1U << 5)) { // If echo detected on PA5
            ultrasonic_detected = 1;
        }

        // LDR Sensor on PB0 (Analog) and PB1 (Digital)
        ADC1->CR2 |= (1 << 30); // Start ADC conversion
        while (!(ADC1->SR & (1 << 1))) {}  // Wait until ADC conversion is complete
        adcData = ADC1->DR;  // Read ADC value

        // Check LDR threshold or digital input
        if ((adcData <= threshold)) {
            ldr_detected = 1;
        }

        // Set or clear the LED on PA6 based on LDR detection
        if (ldr_detected) {
            GPIOA->ODR |= (1U << 6); // Turn on LED on PA6
        } else {
            GPIOA->ODR &= ~(1U << 6); // Turn off LED on PA6
        }

        // Set or clear the LED on PA10 based on PIR detection
        if (pir_detected) {
            GPIOA->ODR |= (1U << 10); // Turn on LED on PA10
        } else {
            GPIOA->ODR &= ~(1U << 10); // Turn off LED on PA10
        }

        // Set or clear the LED on PA7 based on Ultrasonic detection
        if (ultrasonic_detected) {
            GPIOA->ODR |= (1U << 7); // Turn on LED on PA7
        } else {
            GPIOA->ODR &= ~(1U << 7); // Turn off LED on PA7
        }

        // Touch Sensor
        if (GPIOA->IDR & (1U << 8)) { // If touch sensor is triggered
            GPIOA->ODR |= (1U << 9); // Turn on LED on PA9
						USART2_SendString("Touch Sensor Activated\r\n");
        } else {
            GPIOA->ODR &= ~(1U << 9); // Turn off LED on PA9
        }

        // Send messages to Bluetooth terminal
        

        if (ultrasonic_detected) {
            USART2_SendString("Ultrasonic Sensor Activated\r\n");
        }

        if (ldr_detected) {
            USART2_SendString("LDR Sensor Activated\r\n");
        }
				
				

        // USART Communication Handling
        if (USART2->SR & (1 << 5)) { // Check if data is received
            char received = USART2_Read(); // Read character from USART

            if (received == '1') {
                GPIOB->ODR |= (1 << 10);   // Turn on the LED (PC13)
                USART2_SendString("LED ON\r\n"); // Send confirmation
            } else if (received == '0') {
                GPIOB->ODR &= ~(1 << 10);  // Turn off the LED (PC13)
                USART2_SendString("LED OFF\r\n"); // Send confirmation
            } else if (received == '2') {
                GPIOA->ODR |= (1 << 6);
								GPIOA->ODR |= (1 << 7);
								GPIOA->ODR |= (1 << 9);
								GPIOB->ODR |= (1 << 10);
								GPIOA->ODR |= (1 << 10);
								
								USART2_SendString("All LEDs ON\r\n");
								delay(2000); // Delay for 2000 ms (2 seconds)

							// Turn off all LEDs
							GPIOA->ODR &= ~(1 << 6);  // Turn off LDR LED
							GPIOA->ODR &= ~(1 << 7);  // Turn off Ultrasonic LED
							GPIOA->ODR &= ~(1 << 9);  // Turn off Touch LED
							GPIOB->ODR &= ~(1 << 10);  // Turn off motion LED (PC13)
							GPIOA->ODR &= ~(1 << 10); // Turn off PIR LED

							USART2_SendString("All LEDs OFF\r\n");
            } else if (received == '3') {
								GPIOA->ODR |= (1 << 9);
							delay(300); 
								GPIOA->ODR |= (1 << 10);
							delay(300); 
								GPIOA->ODR |= (1 << 7);
							delay(300); 
								GPIOA->ODR |= (1 << 6);
							delay(300); 
								GPIOB->ODR |= (1 << 10);
							delay(300);
							
							GPIOA->ODR &= ~(1 << 6);  // Turn off LDR LED
							GPIOA->ODR &= ~(1 << 7);  // Turn off Ultrasonic LED
							GPIOA->ODR &= ~(1 << 9);  // Turn off Touch LED
							GPIOB->ODR &= ~(1 << 10);  // Turn off motion LED (PC13)
							GPIOA->ODR &= ~(1 << 10); // Turn off PIR LED
							
							delay(300);
								GPIOA->ODR |= (1 << 9);
							delay(300); 
								GPIOA->ODR |= (1 << 10);
							delay(300); 
								GPIOA->ODR |= (1 << 7);
							delay(300); 
								GPIOA->ODR |= (1 << 6);
							delay(300); 
								GPIOB->ODR |= (1 << 10);
							delay(300);
							
							GPIOA->ODR &= ~(1 << 6);  // Turn off LDR LED
							GPIOA->ODR &= ~(1 << 7);  // Turn off Ultrasonic LED
							GPIOA->ODR &= ~(1 << 9);  // Turn off Touch LED
							GPIOB->ODR &= ~(1 << 10);  // Turn off motion LED (PC13)
							GPIOA->ODR &= ~(1 << 10); // Turn off PIR LED
							
							delay(300);
								GPIOA->ODR |= (1 << 9);
							delay(300); 
								GPIOA->ODR |= (1 << 10);
							delay(300); 
								GPIOA->ODR |= (1 << 7);
							delay(300); 
								GPIOA->ODR |= (1 << 6);
							delay(300); 
								GPIOB->ODR |= (1 << 10);
							delay(300);
							
							

							GPIOA->ODR &= ~(1 << 6);  // Turn off LDR LED
							GPIOA->ODR &= ~(1 << 7);  // Turn off Ultrasonic LED
							GPIOA->ODR &= ~(1 << 9);  // Turn off Touch LED
							GPIOB->ODR &= ~(1 << 10);  // Turn off motion LED (PC13)
							GPIOA->ODR &= ~(1 << 10); // Turn off PIR LED

							
            }
						
        }

        // Optional: Add a small delay to debounce the touch sensor
        delay(10);
    }
}

void init_gpio(void) {
    RCC->AHB1ENR |= (1U << 0); // Enable GPIOA clock
    RCC->AHB1ENR |= (1U << 1); // Enable GPIOB clock
    RCC->AHB1ENR |= (1U << 2); // Enable GPIOC clock

    // Configure PA0 as input (Push button)
    GPIOA->MODER &= ~(3U << (2 * 0)); // Clear mode bits for PA0
    GPIOA->PUPDR &= ~(3U << (2 * 0)); // Clear pull-up/pull-down bits for PA0
    GPIOA->PUPDR |= (1U << (2 * 0)); // Set pull-up resistor for PA0

    // Motion Sensor (PIR) on PA11
    GPIOA->MODER &= ~(3U << (2 * 11)); // Set PA11 as input

    // Ultrasonic Sensor
    GPIOA->MODER |= (1U << (2 * 4));  // Set PA4 as output (Trigger)
    GPIOA->MODER &= ~(3U << (2 * 5)); // Set PA5 as input (Echo)

    // LDR Sensor
    GPIOB->MODER |= (3U << 0); // Set PB0 to analog mode
    GPIOB->MODER &= ~(3U << (2 * 1)); // Set PB1 as input (D0 of LDR)

    // LEDs
    GPIOA->MODER |= (1U << (2 * 10)); // Set PA10 as output for PIR LED
    GPIOA->MODER |= (1U << (2 * 6));  // Set PA6 as output for LDR LED
    GPIOA->MODER |= (1U << (2 * 7));  // Set PA7 as output for Ultrasonic LED
    GPIOB->MODER &= ~(3 << (10 * 2)); // Clear mode for PC13
    GPIOB->MODER |= (1 << (10 * 2));  // Set PC13 to output mode

    // Touch Sensor
    GPIOA->MODER &= ~(3U << (2 * 8)); // Set PA8 as input
    GPIOA->PUPDR &= ~(3U << (2 * 8)); // Clear pull-up/pull-down bits for PA8
    GPIOA->PUPDR |= (1U << (2 * 8));  // Set pull-up resistor for PA8

    GPIOA->MODER &= ~(3U << (2 * 9)); // Clear bits for PA9
    GPIOA->MODER |= (1U << (2 * 9));  // Set PA9 to output mode

    // USART2
    GPIOA->MODER &= ~(3 << (2 * 2)); // Clear mode for PA2 (TX)
    GPIOA->MODER |= (2 << (2 * 2));  // Set PA2 to alternate function mode for USART2 TX
    GPIOA->MODER &= ~(3 << (3 * 2)); // Clear mode for PA3 (RX)
    GPIOA->MODER |= (2 << (3 * 2));  // Set PA3 to alternate function mode for USART2 RX

    GPIOA->AFR[0] &= ~(0xF << (2 * 4));  // Clear alternate function for PA2
    GPIOA->AFR[0] |= (7 << (2 * 4));     // Set PA2 to AF7 (USART2)
    GPIOA->AFR[0] &= ~(0xF << (3 * 4));  // Clear alternate function for PA3
    GPIOA->AFR[0] |= (7 << (3 * 4));     // Set PA3 to AF7 (USART2)
}

void init_adc(void) {
    RCC->APB2ENR |= (1U << 8); // Enable ADC1 clock
    ADC1->SQR3 = 8; // ADC1 regular sequence register (channel 8, PB0)
    ADC1->CR2 |= (1U << 0); // Enable ADC1
}

void configure_interrupt(void) {
    RCC->APB2ENR |= (1 << 14);  // Enable SYSCFG clock
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA; // Map EXTI0 line to PA0

    EXTI->IMR |= (1 << 0);  // Unmask EXTI0
    EXTI->FTSR |= (1 << 0); // Trigger on falling edge

    NVIC_EnableIRQ(EXTI0_IRQn); // Enable EXTI0 interrupt in NVIC
}

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & (1 << 0)) { // Check if interrupt was triggered by EXTI0
        start_system = 1; // Set the flag to start the system
        EXTI->PR |= (1 << 0); // Clear interrupt pending bit for EXTI0
    }
}

void USART2_Init(void) {
    RCC->APB1ENR |= (1 << 17); // Enable USART2 clock

    USART2->BRR = 0x0683;      // Set baud rate to 9600 (assuming 16 MHz clock)
    USART2->CR1 |= (1 << 13);  // Enable USART
    USART2->CR1 |= (1 << 2);   // Enable receiver
    USART2->CR1 |= (1 << 3);   // Enable transmitter
}

char USART2_Read(void) {
    while (!(USART2->SR & (1 << 5))); // Wait until data is received
    return USART2->DR;  // Read data from DR register
}

void USART2_Write(char c) {
    while (!(USART2->SR & (1 << 7))); // Wait until TX buffer is empty
    USART2->DR = c; // Write data to DR register
}

void USART2_SendString(char *str) {
    while (*str) {
        USART2_Write(*str++); // Send characters one by one
    }
}