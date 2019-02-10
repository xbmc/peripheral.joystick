# Contributing

Several ways you can contribute to this project.

## Share button maps

#### 1. Check out existing button maps

You can see the existing button maps here:

* [XML button maps](https://github.com/xbmc/peripheral.joystick/tree/master/peripheral.joystick/resources/buttonmaps/xml)

#### 2. Create the button map

Button maps can be generated from within the GUI. See the following wiki articles:

* **[HOW-TO: Configure controllers](https://kodi.wiki/view/HOW-TO:Configure_controllers)**
* **[HOW-TO: Map multiple controllers](https://kodi.wiki/view/HOW-TO:Map_multiple_controllers)**

These map GUI elements to XML files. The XML files can be found in [userdata](https://kodi.wiki/view/Userdata):

```
userdata/addon_data/peripheral.joystick/resources/buttonmaps/xml/<joystick driver>/
```

#### 3. Submit the button map

Fork [peripheral.joystick](https://github.com/xbmc/peripheral.joystick) and clone it to your working directory. Copy the XML file created by the GUI (or by hand) to the folder:

```
peripheral.joystick/resources/buttonmaps/xml/<joystick driver>/
```

Commit the file and send the pull request to the [peripheral.joystick](https://github.com/xbmc/peripheral.joystick) repo.

## Create controller profiles

If you want a new awesome controller to be shown in the GUI, you can create one yourself.

#### 1. Check out existing controller profiles

The controllers in the GUI are defined by Kodi add-ons. You can see the current set of add-ons in the [controller topology project](https://github.com/kodi-game/controller-topology-project):

* [Kodi controller add-ons](https://github.com/kodi-game/controller-topology-project/tree/master/addons)

#### 2. Create a new controller profile

Controllers can be added by creating a controller add-on. The format is defined here:

* [Controller add-on format](https://github.com/kodi-game/controller-topology-project/blob/master/Readme-Addons.md)
