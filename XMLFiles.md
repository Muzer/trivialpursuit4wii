# Introduction #

There will be at least three XML files during normal operation.

fat3:/trivial\_pursuit/settings.xml - The settings file for the app itself

fat3:/trivial\_pursuit/questions\_database\_default.xml - The default questions database

fat3:/trivial\_pursuit/questions\_settings\_default.xml - The settings (eg category names etc) for the default questions

It will be possible to grab questions XMLs from the internet (possibly from the app?). It will detect any file in the trivial\_pursuit folder called questions\_settings**_.xml . It will then go into that file and grab the name for the database, and display a list of these on the screen. Once you select the one you want, it will first look on a specific server if there is a new version of that database (it will check a simple version.txt file). If there is a new version, it will download it. It will then load the questions\_settings file and the associated database file into RAM, as a struct._**

# Details #

## Settings.xml ##

This file is currently undecided

## questions\_database**_.xml ##_

It will be in this format:**

```
<?xml version="1.0"?>
<summary biggestid="1" />
<question id="0" text="Hello, world?" possibleanswers="3" answer1="Hello, world!" answer2="Have fun, world!" answer3="Goodbye, world!" correctanswer="1" category="0" hasimage="1" hasaudio="0" hasvideo="0" imagepath="fat3:/trivial_pursuit/questions_media_default/image0.png" />
<question id="1" etc etc />
```

Possible answers is maximum 6.

## questions\_settings**_.xml ##_

This format:**

```
<?xml version="1.0"?>
<info version="1.0" name="Hello world database" language="en_GB" biggestcatid="1" />
<category id="0" name="Hello, World!" colour="blue" />
<category id="1" etc etc />
```

The "language" string should be in the ISO639-1\_ISO3166 format (ISO639-1 being language code, IOS3166 being country code). ISO639-1 language codes: http://www.loc.gov/standards/iso639-2/php/English_list.php . ISO3166 country codes: http://www.iso.org/iso/country_codes/iso_3166_code_lists/english_country_names_and_code_elements.htm .

For example, for French (France), the code will be fr\_FR. For German in Switzerland, it will be de\_CH.