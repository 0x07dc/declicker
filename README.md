# Declicker

This is an example of a declicker using three algorithms, created using the JUCE C++ framework.

## Running

To run this plugin, one needs a copy of JUCE, of which a free license is available. Simply clone the repository and open the .projucer file with Projucer from the JUCE framework. The project is configured with XCode and Visual Studio exporters, although adding others of the available options is a simple task available in Projucer.

## Repository Structure

The JUCE project is located in the folder, "Declicker." Included in the root folder is a screenshot of the plugin.

## Features

- Three algorithms in sequence (smoothening algorithm, max inter-sample derivative, and max inter-sample second derivative)
- Persistent state for saving/loading music projects and saving/loading plugin presets
- No memory leaks; everything is RAII and thread-safe.