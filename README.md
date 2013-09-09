PROJECT NAME: STOPT
================================================================================
- AUTHOR:   Zagnoyko Dmitry (hiroshidi@gmail.com)  
- LICENCE:  GPLv3    
- L18n:     RU
- TOOLS:    MS VS C++ 2010 Express
- LIBS:     OpenGL

DESCRIPTION:
--------------------------------------------------------------------------------
System for topological optimisation. Uses area dimensions and hotspots radius of 
coverage for calculating total area coverage. Analizes given variants of hotspots
and finds best of them.

BUILD: 
--------------------------------------------------------------------------------
To build project, open math_visualizer.sln in MS VS C++ 2010 or later and do 
'build'.

FEATURES:
--------------------------------------------------------------------------------
- Result is count and cost for hotspots and overlap coeficient
- 2D and 3D modes: for flat area (uses GDI) and volume rooms (uses OpenGL)
- 3D mode supports projections views
- Manual (user puts hotspots) and automatic (program puts hotspots) modes
- Saves resulting array of points as plain text (*.txt)
- Only 2D mode: generated image can be saved in vector graphic (*.emf) file