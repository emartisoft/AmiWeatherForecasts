# AmiWeatherForecasts
This is a weather forecasting application that provides you 
with the latest weather information for any location. 
The application was built using the C programming language 
on Amiga OS 3.2 via SAS/C to retrieve weather information. 

Weather forecast information is updated every 15 minutes. 
It synchronizes the system time using sntp,
if avaliable an internet connection when executing firstly.
Announce time and weather forecast via espeak app.
It has feature that finding of your location automatically.
For App's settings, Click to AmiWFPrefs In Prefs drawer.

It sets Environment variables (CurrentTemperature, Location and 
WeatherDescription)

Tested on OS3.2 & OS3.9 only. It may be work on other WB.

![full](https://github.com/emartisoft/AmiWeatherForecasts/blob/main/screenshots/main_1.6.png?raw=true)

License: MIT

Version History
===============
1.6 (20.01.2024)
- Added turkish catalog (if you can create 
catalog for your language, contact to me on GitHUB)
- Fixed black screen 
- Added invisible style
- Announce time and weather forecast via espeak app
- For App's settings, Click to AmiWFPrefs In Prefs drawer.
  
1.5 (10.12.2023)
- Added bar, digital and iconic styles
- Added select the window and drag to the new position for all styles
- Added environment variables (CurrentTemperature, Location and WeatherDescription) for other applications to use
- The application interface has been made more understandable using tabs
  
1.4 (26.11.2023)
- Encoding problem is solved
- Some languages removed
- Fixed any icons can be moved via drag and drop on WB
- Get Days and Months from locale
- Added use or not abbreviations for days and months
- Fixed the issue of color changes using system palette
- Removed calibration setting for height of the window
- Added 'Add to Pos Y of Text' button
  
1.3 (17.11.2023)
- Using screen text font for GUI (Prefs/Font)
- Added customize color settings
- Changed min and max values for 'Add Pos X and Y'
- Added feature that finding of your location automatically

1.2 (04.11.2023)
- According to SnoopDos, continuous querying of
the TZ variable is blocked.
- Added set date and time manually menu
- Added sync system time using sntp menu
- It synchronizes the system time using sntp if avaliable
an internet connection when executing firstly.

1.1 (29.10.2023)
- Added new look menus
- Added settings
- Added this guide
- Added update menu to refresh weather forecasts immediately 
- Added customized display information for icon, location, 
description and datetime
- Added calibration setting for x and y position and height 
of the window.
- Added menu items for each time your Amiga is booted, this 
application is executed or not. 

1.0 Feb 2022
- First but not published.


