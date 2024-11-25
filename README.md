# 2DNesting
## Description
This software attempts to solve some simple two-dimensional irregular strip packing problems.

![image](https://github.com/user-attachments/assets/3469f341-5c5b-43ee-93db-96521831f84c)

Here are two examples: 

![example1](https://github.com/lryan599/2DNesting/assets/77947703/bc9babaf-d8ee-4fea-a4ad-944ac4ebf495)

![example2](https://github.com/lryan599/2DNesting/assets/77947703/62cb334d-184d-49cd-b552-52b9766b3761)

Experiment results on ESICUP datasets:

![results](https://github.com/lryan599/2DNesting/assets/77947703/3cbc7f79-6b2b-463c-9473-8f5c38939c82)

This software can be divided into an engine part and a UI part. The engine is mainly based on CGAL, and the UI is mainly based on Qt. In addition to these two libraries, there are also some other open source libraries applied to this project.

- [CGAL](https://github.com/CGAL/cgal) 5.6.1, You need to ensure that you can use CGAL correctly and download all related dependencies, including boost, libgmp, libmpfr, etc. For details, please refer to the introduction in the CGAL project to ensure that **you can run a demo of CGAL first**.
- Boost 1.84, Famous C++libraries, please go to the official website to download.
- Qt 6.5.3, The following modules in Qt are required: core, gui, widgets, opengl, openglwidgets, svg, xml, charts, Please visit the Qt official website and refer to the Qt documentation for installation.
- [libdxfrw](https://github.com/LibreCAD/libdxfrw), Used for simple parsing of DXF files.
- [svg_helper](https://github.com/sorrowfeng/svgHelper), Used for simple parsing of SVG files, which is already included in this Git (svd_helper.h, svg_helper.cpp).
- [rapidcsv](https://github.com/d99kris/rapidcsv), Used for simple parsing of CSV files, already included in this Git (csv.h).
- [wyhash](https://github.com/wangyi-fudan/wyhash), already included in this Git (wyhash.h).
- [emhash](https://github.com/ktprime/emhash) 2.1.0, already included in this Git (lru_size.h).

Due to CGAL using the GPL3.0 license, this project can only use the GPL3.0 license. There will be no further major updates for this project, but you are free to ask any questions.
