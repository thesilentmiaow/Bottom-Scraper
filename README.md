# Bottom-Scraper
Arduino code to determine when back of my van will scrape bottom

The sensor is an ultra sound HCSR04 sensor located at the front of the vehicle and pointing vertically down.

The ST7735 display shows a continuous stream of echo responses and there's also a beeping sound to inidcate when we're getting close to 
a risk of bottoming out at the back.

NB I added code to cope with a rear sensor as well - useful if reversing - but the voltage drop was too great for the length of cable so 
never fully implemented that bit.
