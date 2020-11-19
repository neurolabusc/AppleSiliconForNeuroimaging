## About

Historically, Macintosh computers running macOS have been popular with brain imaging scientists. Since they run Unix, users can often use the same tools as the Linux workstations and super computers. The recent [Windows Subsystem for Linux (WSL)](https://www.nemotos.net/?p=1481) has allowed Windows computers to run seamlessly run Unix. Additionally, recent Macintosh computers have switched from Intel x86-64 processors to the ARM-based M1 `Apple Silicon`. This new architecture must translate existing code, and optimal performance will require tools to be recompiled. This page examines the (future) potential for these computers to be used in brain imaging. 

## TL;DR

Unless you are a developer, I would strongly discourage scientists from purchasing a Apple Silicon computer in the short term. Productive work will require core tools to be ported. In the longer term, this architecture could have a profound impact on science, in particular if Apple develops servers that exploit the Metal language and GPUs for compute tasks (competing with NVidia's Tesla products and CUDA language).

## Limitations facing Apple Silicon

The infrastructure scientists depend on is not yet available for this architecture. Here are some of the short term limitations:

 - Scientists using the popular [R](https://developer.r-project.org/Blog/public/2020/11/02/will-r-work-on-apple-silicon/index.html) and Python languages require an open source [Fortran compiler](https://walkingrandomly.com/?p=6696). While Python is available on Apple Silicon, tools like Numpy and [Scipy](https://github.com/scipy/scipy/wiki/Dropping-support-for-Accelerate) require Fortran compilation.
 - Current Apple Silicon compilers do [not support OpenMP](https://github.com/Homebrew/brew/issues/7857). Many scientific tools use this framework to implement parallel computing. This means most native tools will only be able to use a single CPU at a time (though note translated x86-64 OpenMP applications will run multi-threaded).
 - Many [homebrew components](https://github.com/Homebrew/brew/issues/7857) do not support Apple Silicon. Users will have to [install translated components](https://soffes.blog/homebrew-on-apple-silicon).
 - MATLAB is used by many scientific tools, including SPM. While [Matlab](https://www.mathworks.com/matlabcentral/answers/641925-is-matlab-supported-on-apple-silicon-macs) works in translation, it is not yet available natively (and mex files will need to be recompiled).
 - [FSL](https://www.jiscmail.ac.uk/cgi-bin/wa-jisc.exe?A2=ind2011&L=FSL&O=D&X=E5496FE3694704BA21&Y=rorden%40sc.edu&P=163954) and AFNI do not yet natively support this architecture. While code may work in translation, native tools supporting features like OpenMP will require dependencies mature. This will likely require months.
 
There are some inherent weaknesses to using Macintosh for brain imaging

 - NVidia graphics cards and therefore the CUDA language are not supported. Tools like [eddy](https://fsl.fmrib.ox.ac.uk/fsl/fslwiki/eddy), [Probtrackx](https://users.fmrib.ox.ac.uk/~moisesf/Probtrackx_GPU/index.html) and [Bedpostx](https://users.fmrib.ox.ac.uk/~moisesf/Bedpostx_GPU/Installation.html) are dramatically accelerated by this technology.
 - While Linux is open-source, Apple's software are opaque. While some of Apple's attempts to protect users computers may be well intentioned, they have had [unintended consequences](https://arstechnica.com/gadgets/2020/11/macos-big-sur-launch-appears-to-cause-temporary-slowdown-in-even-non-big-sur-macs/). A primary tenet of science is replication, and therefore transparent open source code has strong appeal.
  
## Potential for Apple Silicon

Initial reviews have suggested [Apple's M1 CPU has outstanding performance](https://www.anandtech.com/show/16252/mac-mini-apple-m1-tested). There are specific features that are appealing for neuroimaging.

 - Of particular note for brain scientists is the incredible memory bandwidth. Our field has large datasets that tend to be memory rather than computation bound (the [memory wall](https://www.blosc.org/docs/StarvingCPUs-CISE-2010.pdf)).
 - While some operations lend themselves to the CPU, others are ideal for the GPU. An analogy would be transport via jet plane (CPU: moving a small amount fast), or super tanker (GPU: moving a huge amount with longer latency). However, many operations require sharing data between the [CPU and GPU](https://www.blosc.org/docs/StarvingCPUs-CISE-2010.pdf) so  [memory transfer](https://www.jiscmail.ac.uk/cgi-bin/wa-jisc.exe?A2=ind2004&L=FSL&P=R99017&X=F11DC25F019ED07848&Y=rorden%40sc.edu) can be the bottleneck. In contrast, the Apple M1 chip has unified memory, allowing the GPU and CPU to share memory without penalty.

## Testing

Four computers were tested:

 - MacBook Pro (2018) with Intel [i5--8259u](https://ark.intel.com/content/www/us/en/ark/products/135935/intel-core-i5-8259u-processor-6m-cache-up-to-3-80-ghz.html) (28w) four-core, eight-thread, integrated GPU.
 - Ubuntu 18.04 Intel [i7-8700k](https://ark.intel.com/content/www/us/en/ark/products/126684/intel-core-i7-8700k-processor-12m-cache-up-to-4-70-ghz.html) (95w) six-core, twelve-thread with NVidia GTX [1080 Ti](https://www.geforce.com/hardware/desktop-gpus/geforce-gtx-1080-ti/specifications) (250w) GPU.
 - Ubuntu 20.04 Ryzen []() (105w) twelve-core, twenty-four-thread  with NVidia GTX [1080 Ti](https://www.geforce.com/hardware/desktop-gpus/geforce-gtx-1080-ti/specifications) (250w) GPU.
 - MacBook Air (2020) with Apple Silicon M1 with four fast cores plus four efficiency cores and integrated GPU.


## pigz

The GZip compression format is a staple of neuroimaging, with NIfTI images often stored in GZip format. The compression is slow (relative to decompression), but the pigz tool can use multiple threads to accelerate compression. Indeed, popular tools like AFNI and dcm2niix can use pigz. Since pigz does not use OpenMP, it is easy to natively compile for Apple Silicon. A previous review of the Apple M1 CPU examined [pigz](https://arstechnica.com/gadgets/2020/11/hands-on-with-the-apple-m1-a-seriously-fast-x86-competitor/), however that review only examined the conventional version. Here we compare both conventional pigz with an accelerated version using the [CloudFlare zlib](https://github.com/cloudflare/zlib) which leverages dedicated hardware in both Intel and ARM computers. Here we tested compression of the [Silesia compression corpus](http://sun.aei.polsl.pl/~sdeor/index.php?page=silesia).

The graph shows compression performance as megabytes per second, with higher values indicating faster performance. The horizontal axes shows the number of threads, with actual values for 1..4 and 5 representing the use of all available threads (e.g. 8 threads for the M1 but 24 for the 3900X). The shaded color for each line illustrates the performance difference between the standard pigz (bottom of shaded region) and the CloudFlare pigz (top of shaded region).

![pigz](pigz.png)

## fslmaths

fslmaths handles much of the internal processing for FSL, and is a popular stand-alone tool for computation. While [FSL](https://www.jiscmail.ac.uk/cgi-bin/wa-jisc.exe?A2=ind2011&L=FSL&O=D&X=E5496FE3694704BA21&Y=rorden%40sc.edu&P=163954) is not yet compiled for Apple Silicon, translations run seamlessly. On the other hand, the [niimath](https://github.com/rordenlab/niimath) clone can be compiled for Apple Silicon. Of specific interest, niimath for x86-64 can be compiled for OpenMP, providing a glimpse for parallel processing. Parallel processing is used for some computation and for saving data (using pigz). Note that when run in parallel on the M1, niimath uses translated x86-64 code for computations but native CloudFlare pigz for data compression. The graph shows the geometric mean for four computations (de-meaning, temporal filtering, Gaussian smoothing, image dilation).

![fslmaths](fslmaths.png)

## MRIcroGL

[MRIcroGL](https://github.com/rordenlab/MRIcroGL12/releases) is used for visualization of neuroimaging data. It is available for Linux, Windows and macOS. For macOS, a universal binary is provided that runs natively on both x86-64 and Apple Silicon. For Windows and Linux, OpenGL is used for visualization and some compute. For macOS versions are available for both OpenGL and Metal. The latter was used here (while deprecated on macOS, OpenGL performs well, albeit a bit slower). Testing involved a [digimorph desert iguana](http://digimorph.org/specimens/Dipsosaurus_dorsalis/) that used two 512mb textures of GPU memory. 

![mricrogl](iguana.jpg)

The first test examines the CPU performance, looking at the loading, scaling and transformation of the image data.

![mricrogl](mricrogl.png)

The second test examines the GPU compute performance, looking at generation of the intensity [gradients](https://www.mccauslandcenter.sc.edu/mricrogl/gradients).

![mricrogl](mricrogl_gpu.png)

