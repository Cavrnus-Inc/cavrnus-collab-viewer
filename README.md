
<p align="center">
    <img src="ReadmeContent/banner.png" alt="Cavrnus Banner"/>
</p>

# <p style="text-align: center;">Cavrnus Collab Viewer</p>

<h4 align="center">
  <a href="https://www.cavrnus.com/">
    <img src="https://img.shields.io/badge/Cavrnus%20Website-label?style=flat&color=white" alt="Cavrnus Website" height="20">
  </a>
  <a href="https://www.youtube.com/@cavrnus">
    <img src="https://img.shields.io/badge/Cavrnus%20YouTube-label?style=flat&logo=YouTube&logoColor=red&labelColor=white&color=white" alt="Cavrnus YouTube" height="20">
  </a>
  <a href="https://twitter.com/cavrnus">
    <img src="https://img.shields.io/badge/Cavrnus_Twitter-label?style=flat&logo=x&logoColor=black&labelColor=white&color=white" alt="Cavrnus Twitter" height="20">
  </a>
  <a href="https://discord.gg/AzgenDT7Ez">
    <img src="https://img.shields.io/badge/Cavrnus_Support-label?style=flat&logo=discord&labelColor=white&color=white" alt="Cavrnus Discord" height="20">
  </a>
</h4>

<h4 align="center">
  <img src="https://img.shields.io/badge/Version-1.0.0-blue?style=flat&labelColor=blue&color=white" alt="Version 1.0.0" height="20">
</h4>

    
## Project Information  
**Version:** 2024.2.0    
**Last Updated:** 2024-12-05    
**Documentation:** [Cavrnus Collaborative Viewer Documentation](https://cavrnus.atlassian.net/wiki/spaces/CSM/pages/948764675/Cavrnus+with+the+Unreal+Collab+Viewer+Template+CVT)     

## Introduction

### Overview
The CavrnusCVT plugin offers a seamless way to integrate multi-user collaboration, real-time communication, and Datasmith synchronization into CVT Unreal Engine template projects or even mature, established projects. By simply dragging the Cavrnus plugin into the CVT project, users can effortlessly enhance 3D workflows and embed collaborative features directly into their Unreal projects.

### Project Contents
This repo includes both the CavrnusCVT plugin and the corresponding CavrnusConnector plugin. These must live together in your project's plugin folder in order for proper functionality.

### Requirements
- Unreal Engine 5.3.x

## Getting Started

### Opening Git Project
1. Clone the project using your preferred Git client.
2. Open the project in Unreal Engine 5.x.x.
3. When prompted, opt to build the required modules.
4. Follow the prompts during the build process (note: it is not necessary to generate a project solution).
Once the build completes, the project should open automatically.

## Installing in Other Projects
Need CVT features in other projects? It's easy to use the power of Cavrnus and CVT by simply dragging the plugins into other projects.

1. Locate plugins in this project
    * a. Find the /Plugins folder located in root project folder.
    * b. Select/Copy both the CavrnusConnector and CavrnusCVT folders.
        <p align="center"><img src="ReadmeContent/plugin-folders.png"/></p>

2. Navigate to the target project you would like to add these plugins to.
3. In the target project's root folder, locate the Plugins folder (create one if it doesn't exist).
    * a. Paste both the CavrnusConnector and CavrnusCVT folders into this folder.

4. Open the project
    * a. Either use the .uproject file as normally and opt to rebuild the new modules.
    * b. Or regenerate the solution files if it is a C++ project.

### Setup

1. Once your project is open, navigate to <b>Edit > Plugins</b>. This opens the <b>Plugins</b> window.

2. Verify the <b>CavrnusConnector</b> and <b>CavrnusCVT</b> plugins are present and enabled.
        <p align="center"><img src="ReadmeContent/plugins.png"/></p> 

### Quick Start Guide
Starting from an existing level is easy!

1. Navigate to and select <b>CavrnusCVT > Setup level for Cavrnus & CVT</b>.
    <p align="center"><img src="ReadmeContent/dropdown.png"/></p> 

2. Your level now has the necessary Cavrnus actors.
    <p align="center"><img src="ReadmeContent/added-actors.png"/></p> 

3. In the level Outliner, select the CavrnusSpatialConnector instance.
    <p align="center"><img src="ReadmeContent/connector-outliner.png"/></p> 


2. From the Details panel, you can now configure all of your connection settings for this project. For in-depth help with setting up the Cavrnus Spatial Connector actor, see [Configure the Cavrnus Spatial Connector](https://cavrnus.atlassian.net/wiki/spaces/CSM/pages/872742940/Configure+the+Cavrnus+Spatial+Connector+Unreal+Engine) documentation.
    <p align="center"><img src="ReadmeContent/connector.png"/></p> 


### **Full Cavrnus Collab Viewer Integration Guide** 
This <b>CavrnusCVT</b> project also includes an in-depth guide of the [Cavrnus Collab Viewer integration process](https://cavrnus.atlassian.net/wiki/spaces/CSM/pages/1048182803/Cavrnus-CVT-Plugin+Setup).

For more information on the underlying <b>CavrnusConnector</b> plugin of which the Cavrnus Collab Viewer plugin is built upon, refer to [Cavrnus Spatial Connector for UnrealEngine](https://cavrnus.atlassian.net/wiki/spaces/CSM/pages/872808449/Cavrnus+Spatial+Connector+for+Unreal+Engine) documentation.


## Support and Feedback
Do you need help? Have you found a bug? Reach out through the [Cavrnus Discord](https://discord.gg/AzgenDT7Ez).