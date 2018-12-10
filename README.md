<h1> Bus Reminder </h1>

<h2>Purpose</h2>
<p>I'd like to think I'm constantly on top of when I need to leave for class, but the reality is that I often miss the bus and that means being a couple minutes late. While I could pull out my phone and check the time to the next bus while running around the kitchen, why would I when I could spend a lot of time debugging, soldering, and tinkering to create an ESP8266 powered bus reminder?</p>

<h2>Description</h2>
<p>This project fetches and displays the time until the next bus arrives at the stop number of your choice. It only works for the Grand River Transit system, and relies on <a href="https://github.com/nwoodthorpe/GRTUnofficialLiveAPI">Nathaniel Woodthorpe's API</a> for retrieving GRT transit data. The code runs on an ESP8266 which hits the API, parses data and translates that into the bytes needed to be shifted out to the shift register, which then displays them on a 4 digit 7 segment display (uses multiplexing to do so). 

Display displays the route number for a set period of time, with a rotating animation that distinguishes the route number from the timer. Then displays the timer.

You can find the design for the box I 3D printed <a href="https://a360.co/2RK5xdG" >here.</a>
<h2>Materials</h2>
<ul>
<li> ESP8266 12E (Mine is Node MCU's board)
<li> 4 Digit 7 Segment display (Mine is labeled 3461BS and is Common Anode)
<li> SN74HC595N Shift Register 
<li> 4 X 220 <span>&#8486;</span> resistors
<li> Prototype board 
<li> USB Micro B Cable (To connect computer to ESP8266)
</ul>

<h2>Tools</h2>
<ul>
<li> Sodlering iron (and solder)
<li> Arduino IDE
<li> BONUS: If you have or have access to a 3D printer you can make a case as well
</ul>

<h2>Future improvements</h2> 
<ul>
<li> Replace the api with GRT's official API in the GTFS (Google Transit Feed Specifications) format.
<ul> <li> It seems like a more sustainable option long term</li> </ul>
<li> Add options as global vars to top of main.ino so that they are easy to change 
<ul> <li> Display time of routeId and arrival countdown</li></ul>
<li> Add Pictures to readme </li>
<li> Add code to turn off at night, e.g. if there are no busses left, go to sleep until the morning. </li>
</ul>

<h2> Credits </h2>
Big thanks to <a href="https://github.com/nwoodthorpe">Nathaniel Woodthorpe</a> and his creating and hosting of a GRT API.

