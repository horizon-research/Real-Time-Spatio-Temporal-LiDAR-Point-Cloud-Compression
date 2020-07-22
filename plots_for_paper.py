
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.ticker import FuncFormatter
from matplotlib.patches import Ellipse
import numpy as np
import math
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
##############################################################
#                       STEREO PROJECT                       #
##############################################################

def PlotParentoPCC():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(6,4)).add_subplot(111)
    ax1.set_xlabel('Compression rate (bpp)', fontsize=16)
    ax1.set_ylabel('Peak signal-to-noise ratio', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)
    x_axis_ls = []

    # obtained number from the survey paper;
    # pcc_1d = ax1.plot([16.12, 15.51, 10.47, 11.08], [100， 100， 100], color='#f9a903', linestyle='none',\
            # linewidth=1.5,markeredgecolor='k', marker='d', markersize=10);
                                # 1081966140                                 
    pcc_2d_static  = ax1.plot([1.20, 3.41], [87, 60], color='#d08327', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='o', markersize=10);
                                # 1119628511
    pcc_2d_dynamic = ax1.plot([0.001, 0.1, 10, 18, 0.01, 0.07], [28, 31, 99, 55, 29, 39],\
                              color='tomato', linestyle='none', linewidth=1.5, \
                              markeredgecolor='k', marker='p', markersize=10);
                                # 3299386765
    pcc_3d_static = ax1.plot([0.32, 6.1, 0.36, 6], [53, 70, 49, 61], color='#5b87f2', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='v', markersize=10);

    pcc_3d_dynamic = ax1.plot([0.1, 3.2], [86, 65], color='#118014', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='h', markersize=10);

    lidar_img = ax1.plot([0.17, 0.66, 2.57, 15.13], [42.13, 48.29, 56.00, 70.57], color='#884dff', linestyle='none',\
            linewidth=1.5,markeredgecolor='r', marker='*', markersize=14, clip_on=False);
    lidar_ffv1 = ax1.plot([0.13, 0.48, 1.78, 11.65], [42.13, 48.29, 56.00, 70.57], color='#48d479', linestyle='none',\
            linewidth=1.5,markeredgecolor='r', marker='^', markersize=12, clip_on=False);
    lidar_mpeg4 = ax1.plot([0.006, 0.028, 0.038, 0.275], [41.76, 47.65, 45.83, 38.73], color='#4971eb', linestyle='none',\
            linewidth=1.5,markeredgecolor='r', marker='s', markersize=10, clip_on=False);

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    plt.xscale('log')
    plt.subplots_adjust(left=0.17, bottom=0.14, right=0.95, top=0.93,
                wspace=0.2, hspace=0.2)
    # ax1.set_ylim(10000000, 10000000000000)
    ax1.set_xlim(0.0001, 25)
    # plt.xticks([0, 3, 6, 9, 12, 15])

    plt.legend((pcc_2d_static[0], pcc_2d_dynamic[0], pcc_3d_static[0],
    			pcc_3d_dynamic[0], lidar_img[0], lidar_ffv1[0], lidar_mpeg4[0]),
        		('PCC2d-image', 'PCC2d-video', 'PCC3d-image', 'PCC3d-video',
        		 'LiDAR-IMG(our)', 'LiDAR-ffv1(our)', 'LiDAR-MPEG4(our)'), prop={'size': 11})
    
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("comparison.pdf");


def PlotParentoByName():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(6,4)).add_subplot(111)
    ax1.set_xlabel('Compression rate (bpp)', fontsize=16)
    ax1.set_ylabel('Peak signal-to-noise ratio', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)
    x_axis_ls = []

    # obtained number from the survey paper;
    # pcc_1d = ax1.plot([16.12, 15.51, 10.47, 11.08], [100， 100， 100], color='#f9a903', linestyle='none',\
            # linewidth=1.5,markeredgecolor='k', marker='d', markersize=10);
                                # 1081966140                                 
    pcc_2d_static  = ax1.plot([1.20, 3.41], [60, 87], color='#d08327', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='o', markersize=10);
                                # 1119628511
    pcc_2d_dynamic = ax1.plot([0.001, 0.1, 10, 18], [28, 31, 55, 99],\
                              color='tomato', linestyle='none', linewidth=1.5, \
                              markeredgecolor='k', marker='h', markersize=10);

    vpcc_dynamic = ax1.plot([0.01, 0.07], [29, 39],\
                              color='tomato', linestyle='none', linewidth=1.5, \
                              markeredgecolor='k', marker='d', markersize=10);
                                # 3299386765
    pcc_3d_static = ax1.plot([0.36, 6], [49, 61], color='#5b87f2', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='v', markersize=10);

    lpcc_static = ax1.plot([0.32, 6.1], [53, 70], color='#5b87f2', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='X', markersize=10);

    pcc_3d_dynamic = ax1.plot([0.1, 3.2], [65, 86], color='#118014', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='P', markersize=10);

    lidar_img = ax1.plot([0.17, 0.66, 2.57, 15.13], [42.13, 48.29, 56.00, 70.57], color='#884dff', linestyle='none',\
            linewidth=1.5,markeredgecolor='r', marker='*', markersize=14, clip_on=False);
    lidar_ffv1 = ax1.plot([0.13, 0.48, 1.78, 11.65], [42.13, 48.29, 56.00, 70.57], color='#48d479', linestyle='none',\
            linewidth=1.5,markeredgecolor='r', marker='^', markersize=12, clip_on=False);
    lidar_mpeg4 = ax1.plot([0.006, 0.028, 0.038, 0.275], [41.76, 47.65, 45.83, 38.73], color='#4971eb', linestyle='none',\
            linewidth=1.5,markeredgecolor='r', marker='s', markersize=10, clip_on=False);

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    plt.xscale('log')
    plt.subplots_adjust(left=0.17, bottom=0.14, right=0.95, top=0.93,
                wspace=0.2, hspace=0.2)
    # ax1.set_ylim(10000000, 10000000000000)
    ax1.set_xlim(0.0001, 25)
    # plt.xticks([0, 3, 6, 9, 12, 15])

    plt.legend((pcc_2d_static[0], pcc_2d_dynamic[0], vpcc_dynamic[0], pcc_3d_static[0], lpcc_static[0],
                pcc_3d_dynamic[0], lidar_img[0], lidar_ffv1[0], lidar_mpeg4[0]),
                ('Ochotta-2004-image', 'PCC2d-video', 'VPCC', 'PCC3d-image', 'LPCC(octree)', 'octree+img comp.',
                 'LiDAR-IMG(our)', 'LiDAR-ffv1(our)', 'LiDAR-MPEG4(our)'), prop={'size': 11})
    
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("comparison.pdf");


def PlotClusterResult():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(6,4)).add_subplot(111)
    ax1.set_xlabel('Compression rate (bpp)', fontsize=16)
    ax1.set_ylabel('Peak signal-to-noise ratio', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)
    x_axis_ls = []

    # obtained number from the survey paper;
    # pcc_1d = ax1.plot([16.12, 15.51, 10.47, 11.08], [100， 100， 100], color='#f9a903', linestyle='none',\
            # linewidth=1.5,markeredgecolor='k', marker='d', markersize=10);
                                # 1081966140                                 
    cluster2  = ax1.plot([3.2235, 0.809402, 0.1305256, 0.0332182, 0.0085086], \
                          [43.2698, 40.1646, 38.0944, 36.7232, 35.2276], color='#d08327', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='o', markersize=10);
                                # 1119628511
    cluster4 = ax1.plot([6.0562, 1.53184, 0.2490464, 0.0634068, 0.01611204], \
                         [49.6091, 47.0998, 44.6475, 42.9252, 40.5811],\
                              color='tomato', linestyle='none', linewidth=1.5, \
                              markeredgecolor='k', marker='h', markersize=10);

    cluster6 = ax1.plot([7.84182, 1.990794, 0.3234084, 0.0820914, 0.02101464], \
                         [51.9669, 47.5289, 44.0274, 42.0132, 39.9266],\
                              color='tomato', linestyle='none', linewidth=1.5, \
                              markeredgecolor='k', marker='d', markersize=10);
                                # 3299386765
    clsuter8 = ax1.plot([9.53112, 2.429448, 0.3990264, 0.1024144, 0.02648696], \
                        [55.2127, 50.2198, 46.1376, 43.8375, 41.6149], color='#5b87f2', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='v', markersize=10);

    cluster10 = ax1.plot([14.9144, 3.84224, 0.633766, 0.161593, 0.0412438],\
                        [58.8221, 53.806, 49.6771, 47.2567, 44.7774], color='#5b87f2', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='X', markersize=10);

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    plt.xscale('log')
    plt.subplots_adjust(left=0.17, bottom=0.14, right=0.95, top=0.93,
                wspace=0.2, hspace=0.2)
    ax1.set_xlim(0.005, 25)
    # plt.xticks([0, 3, 6, 9, 12, 15])

    plt.legend((cluster2[0], cluster4[0], cluster6[0], clsuter8[0], cluster10[0]),
                ('#clusters=2','#clusters=4','#clusters=6','#clusters=8','#clusters=10'), prop={'size': 11})
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("cluster.pdf");


def PlotDist2Approximation():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(4,4)).add_subplot(111)
    ax1.set_xlabel('Distance-to-Object (m)', fontsize=16)
    ax1.set_ylabel('Deviation (cm)', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)
    x_axis_ls = [5.0*x for x in range(20)]

    norm  = ax1.plot(x_axis_ls, [(x/math.cos(math.pi*0.18/180.0)-((x/math.cos(math.pi*0.09/180.0)-x)*2+x))*100 for x in x_axis_ls], 
                    color='#d08327', linestyle='none',linewidth=1.5,markeredgecolor='k', marker='o', markersize=6);

    # angles = [15.0, 30.0, 45.0, 60.0, 75.0]
    angles = [30.0, 60.0, 75.0]
    markers = ['v', 'X', '^', '>', 'd']
    colors = ['#5b87f2', '#118014', '#884dff', '#4971eb', 'tomato']
    idx=0
    pos = {}

    for angle in angles:
        pos[angle] = ax1.plot(x_axis_ls, [((x/math.sin(math.pi*((90.0-angle)-0.18)/180.0)*math.sin(math.pi*(90+angle)/180.0)) -
                                   ((x/math.sin(math.pi*((90.0-angle)-0.09)/180.0)*math.sin(math.pi*(90.0+angle)/180.0) - x) * 2 + x)) * 100 for x in x_axis_ls], 
                      color=colors[idx], linestyle='none',linewidth=1.5,markeredgecolor='k', marker=markers[idx], markersize=6);
        idx+=1

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    plt.yscale('log')
    plt.subplots_adjust(left=0.25, bottom=0.14, right=0.95, top=0.93,
                wspace=0.2, hspace=0.2)
    ax1.set_xlim(0, 100)
    ax1.set_ylim(0.001, 1)
    # plt.xticks([0, 3, 6, 9, 12, 15])

    # plt.legend((norm[0], pos[15][0], pos[30][0], pos[45][0], pos[60][0], pos[75][0]),
    #             ('norm','deg=15','deg=30','deg=45','deg=60','deg=75'), prop={'size': 11}, loc='lower right', ncol=2)
    plt.legend((norm[0], pos[30][0], pos[60][0], pos[75][0]),
                ('norm','deg=30','deg=60','deg=75'), prop={'size': 11}, loc='lower right', ncol=1)
    

    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("deviation.pdf");



def PlotStereoMismatch():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(6,4)).add_subplot(111)
    ax1.set_xlabel('frame delay (ms)', fontsize=16)
    ax1.set_ylabel('depth estimation error (m)', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)
    x_axis_ls = [0, 67, 133, 200, 266, 330]

    # obtained number from the survey paper;
    # pcc_1d = ax1.plot([16.12, 15.51, 10.47, 11.08], [100， 100， 100], color='#f9a903', linestyle='none',\
            # linewidth=1.5,markeredgecolor='k', marker='d', markersize=10);
                                # 1081966140                                 
    slow  = ax1.plot(x_axis_ls, [1.43, 4.73, 5.61, 5.81, 6.06, 6.82], color='#d08327', linestyle='--',\
            linewidth=1.5,markeredgecolor='k', marker='o', markersize=10);
    # [40.73, 47.35, 53.73, 56.23, 57.59, 58.69]

   
                                # 1119628511
    fast = ax1.plot(x_axis_ls,[1.50, 4.35, 10.93, 20.93, 41.16, 42.51],\
                              color='tomato', linestyle='--', linewidth=1.5, \
                              markeredgecolor='k', marker='h', markersize=10);
    # [29.624, 30.80, 44.04, 67.24, 88.71, 97.22]

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # plt.xscale('log')
    plt.subplots_adjust(left=0.17, bottom=0.14, right=0.95, top=0.93,
                wspace=0.2, hspace=0.2)
    # ax1.set_xlim(0.005, 25)
    # plt.xticks([0, 3, 6, 9, 12, 15])

    plt.legend((slow[0], fast[0]),
                ('slow motion','fast motion',), prop={'size': 11})
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("mismatch.pdf");

def PlotParentoTradeoff():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(6,4)).add_subplot(111)
    ax1.set_xlabel('Compression rate (bpp)', fontsize=16)
    ax1.set_ylabel('RMSE', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)
    x_axis_ls = [0.06, 0.22, 0.51, 1, 1.5, 1.5]
    y_axis_ls = [0.32, 0.12, 0.07, 0.05, 0.03, 0.03]
                              
    octree = ax1.plot(x_axis_ls, y_axis_ls, color='#d08327', linestyle='none',\
            linewidth=1.5,markeredgecolor='k', marker='o', markersize=10)

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # plt.xscale('log')
    plt.subplots_adjust(left=0.17, bottom=0.14, right=0.95, top=0.93,
                wspace=0.2, hspace=0.2)

    ax1.set_xlim(0, 2)
    ax1.set_ylim(0, 0.4)
    # plt.xticks([0, 3, 6, 9, 12, 15])

    # plt.legend((octree[0]), ('octree'), prop={'size': 11})
    
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("tradeoff.pdf");

def PlotRegiestration():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(7,3.5)).add_subplot(111)
    ax1.set_xlabel('Compression Rate', fontsize=16)
    ax1.set_ylabel('Translation Error (%)', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)
    x_axis_ls = []
                              
    # pcc = ax1.plot([ 96/i for i in [3.94, 5.91, 7.87, 11.81, 15.75]], [1.01, 0.982, 0.996, 1.37, 1.121],\
    #                 color='tomato', linestyle='none',\
    #                 linewidth=1.5,markeredgecolor='k', marker='o', markersize=12);

    pcc_stream = ax1.plot([96/i for i in [0.98, 1.08, 1.34, 1.91]], \
                    [3.56, 0.91, 0.916, 0.83],\
                    color='#71985E', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='^', markersize=12);

    pcc_frame = ax1.plot([96/i for i in [1.16, 1.48, 2.05, 2.53]], \
                    [2.02, 1.00, 0.96, 0.923], \
                    color='#FFBF56', linestyle='--', \
                    linewidth=1.5,markeredgecolor='k', marker='^', markersize=12);

    img = ax1.plot([20.55], [3.95],\
                    color='#884dff', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='o', markersize=12);


    plt.annotate('16.3', xy=(21, 3.9), xycoords='data',
             xytext=(20, 3.1), textcoords='data',fontsize=12,
             arrowprops=dict(arrowstyle="->"))

    video_lossy = ax1.plot([105.31], [3.95],\
                    color='#c90a0a', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='D', markersize=8);

    vpcc = ax1.plot([112.43], [3.95],\
                    color='#b86207', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='s', markersize=10);

    plt.annotate('>85%', xy=(105.5, 3.9), xycoords='data',
             xytext=(106, 3.1), textcoords='data',fontsize=12,
             arrowprops=dict(arrowstyle="->"))

    plt.annotate('', xy=(112.5, 3.9), xycoords='data',
             xytext=(112, 3.4), textcoords='data',fontsize=12,
             arrowprops=dict(arrowstyle="->"))

    video_lossless = ax1.plot([5.41], [1.12],\
                    color='#f5704e', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='D', markersize=9);

    octree = ax1.plot([96/i for i in [1.48, 1.80, 2.85, 5.06, 7.87, 11.36]], \
                                     [3.38, 2.30, 1.84, 1.42, 1.39, 1.39], \
                    color='#5b87f2', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='X', markersize=12);

    # oct_n_trisoup = ax1.plot([96/i for i in [8.27, 14.11, 19.78]], [1.77, 1.5, 1.4], \
    #                 color='#16ADB0', linestyle='--',\
    #                 linewidth=1.5,markeredgecolor='k', marker='X', markersize=12);


    org_pipe = ax1.plot([1], [1.25], color='#48d479', linestyle='none',\
            linewidth=1.5, markeredgecolor='k', marker='v', markersize=12, clip_on=False);
    
    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # plt.xscale('log')
    plt.subplots_adjust(left=0.17, bottom=0.15, right=0.95, top=0.77,
                wspace=0.2, hspace=0.2)
    ax1.set_ylim(0, 4)
    ax1.set_xlim(0, 120)
    # plt.xticks([0, 5, 10, 15, 20, 25])

    plt.legend((org_pipe[0], pcc_frame[0], pcc_stream[0], 
                 video_lossy[0], video_lossless[0], img[0],
                 octree[0], vpcc[0]),
                ('Orginal', 'Ours (single-frame)', 'Ours (streaming)',
                  'Video (lossy)', 'Video (lossless)', 'JPEG',
                  'GPCC',  'VPCC'),
                bbox_to_anchor=(0., 1.01, 1., .101), loc=3, fontsize=12,
            ncol=3, mode="expand", borderaxespad=0., frameon=False)
    
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("registration.pdf");


def PlotDetection():
    plt.rc('font', size=14)
    ax1 = plt.figure(figsize=(7,3)).add_subplot(111)
    ax1.set_xlabel('Compression Rate', fontsize=16)
    # ax2 = ax1.twinx()
    ax1.set_ylabel('Det. Accuracy (%)', fontsize=16)
    # ax2.set_ylabel('BEV Accuracy (%)', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)

    pcc_frame = ax1.plot([11.47, 24.43, 42.29, 57.15, 88.89, 104.34], \
                    [72.15, 70.75, 69.55, 65.85, 54.03, 47.29], \
                    color='#FFBF56', linestyle='--', \
                    linewidth=1.5,markeredgecolor='k', marker='^', markersize=12);

    org_pipe = ax1.plot([1], [74.46], color='#48d479', linestyle='none',\
            linewidth=1.5, markeredgecolor='k', marker='v', markersize=12, clip_on=False);

    octree = ax1.plot([5.23, 8.45, 12.19, 33.21, 76.8], \
                      [54.54, 55.29, 53.58, 39.42, 37.38], \
                    color='#5b87f2', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='X', markersize=12);

    jpeg = ax1.plot([20.55, 25.19, 33.45], [57.56, 43.29, 14.15],\
                    color='#884dff', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='o', markersize=12);

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # plt.yscale('log')
    plt.subplots_adjust(left=0.13, bottom=0.18, right=0.98, top=0.85,
                wspace=0.2, hspace=0.2)
    ax1.set_ylim(0, 100)
    ax1.set_xlim(0, 120)
    # plt.xticks([0, 5, 10, 15, 20, 25])
    # plt.xticks([1, 4.5, 9, 12.5], ['Org.', 'Ours', 'G-PCC', 'JPEG'])

    legend1 = plt.legend((org_pipe[0], pcc_frame[0], octree[0], jpeg[0]), \
                ('Orginal', 'Ours (single-frame)','GPCC', 'JPEG'), \
            bbox_to_anchor=(0., 1.01, 1., .101), loc=3,
            ncol=4, mode="expand", borderaxespad=0., frameon=False)

    plt.gca().add_artist(legend1)
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("detection.pdf");

def PlotSegmentation():
    plt.rc('font', size=14)
    ax1 = plt.figure(figsize=(7,3)).add_subplot(111)
    ax1.set_xlabel('Compression Rate', fontsize=16)
    ax1.set_ylabel('Seg. Accuracy (%)', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)

    org_pipe = ax1.plot([1], [76.2], color='#48d479', linestyle='none',\
            linewidth=1.5, markeredgecolor='k', marker='v', markersize=12, clip_on=False);

    pcc_frame = ax1.plot([18.00, 33.01, 52.45, 80.43], \
                    [75.00, 73.6, 72.4, 38.1], \
                    color='#FFBF56', linestyle='--', \
                    linewidth=1.5,markeredgecolor='k', marker='^', markersize=12);

    pcc_stream = ax1.plot([21.82, 25.53, 40, 50.26, 71.64, 88.89], \
                    [75.5, 74.1, 73.6, 72.4, 49.3, 38.1],\
                    color='#71985E', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='^', markersize=12);

    octree = ax1.plot([8.45, 12.20, 24.31, 33.21], \
                      [75.2, 70, 51.8, 33.9], \
                    color='#5b87f2', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='X', markersize=12);

    jpeg = ax1.plot([20.55, 25.19, 33.45, 62.75], [71.5, 69.2, 66.8, 45.8],\
                    color='#884dff', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='o', markersize=12);

    video_lossy = ax1.plot([105.31], [42.6],\
                    color='#c90a0a', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='D', markersize=8);

    video_lossless = ax1.plot([5.41], [75.5],\
                    color='#f5704e', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='D', markersize=9);

    vpcc = ax1.plot([112.43], [71.0],\
                    color='#b86207', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='s', markersize=10);

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # plt.yscale('log')
    plt.subplots_adjust(left=0.13, bottom=0.18, right=0.95, top=0.70,
                wspace=0.2, hspace=0.2)
    ax1.set_ylim(0, 100)
    ax1.set_xlim(0, 120)

    legend = plt.legend((org_pipe[0], pcc_frame[0], pcc_stream[0], \
                        video_lossy[0], video_lossless[0], jpeg[0] , \
                        octree[0], vpcc[0]), \
                        ('Orginal', 'Ours (single-frame)', 'Ours (streaming)', \
                         'Video (lossy)','Video (lossless)', 'JPEG', \
                         'GPCC', 'VPCC',), \
            bbox_to_anchor=(0., 1.01, 1., .101), loc=3,fontsize=12,
            ncol=3, mode="expand", borderaxespad=0., frameon=False)

    plt.gca().add_artist(legend)
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor', direction="in")
    ax1.tick_params(axis='y',which='minor', direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("segmentation.pdf");


def PlotSensitivity():
    plt.rc('font', size=14)
    ax1 = plt.figure(figsize=(7,3)).add_subplot(111)
    ax1.set_xlabel('Number of Frames', fontsize=16)
    ax1.set_ylabel('Norm. Value', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)

    translation = ax1.plot([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], \
            [1.08, 0.97, 1.00, 1.06, 1.20, 1.10, 1.12, 1.16, 1.10, 1.07], \
            color='#189adb', linestyle='--',\
            linewidth=1.5, markeredgecolor='#189adb', \
            markerfacecolor='none', marker='v', markersize=12, clip_on=False);

    segmentation = ax1.plot([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], \
                    [1.01, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 0.99, 0.99],\
                    color='#f26161', linestyle='--',\
                    linewidth=1.5,markeredgecolor='#f26161', \
                    markerfacecolor='none',marker='^', markersize=12);

    rate = ax1.plot([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], \
                    [0.99, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00],\
                    color='#71985E', linestyle='--',\
                    linewidth=1.5,markeredgecolor='#71985E', \
                    markerfacecolor='none',marker='X', markersize=12);

    # runtime_single = ax1.plot([1, 3, 5, 7, 10, 13, 16, 20], \
    #                 [0.91, 0.95, 1, 1.06, 1.066, 1.067, 1.08,  1.08],\
    #                 color='#884dff', linestyle='--',\
    #                 linewidth=1.5,markeredgecolor='#884dff', \
    #                 markerfacecolor='none',marker='o', markersize=12);

    runtime_parallel = ax1.plot([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], \
                    [0.23, 0.91, 1.00, 1.19, 1.34, 1.35, 1.42, 1.47, 1.49, 1.52],\
                    color='#b86207', linestyle='--',\
                    linewidth=1.5,markeredgecolor='#b86207', \
                    markerfacecolor='none',marker='s', markersize=10);

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    # plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # plt.yscale('log')
    plt.subplots_adjust(left=0.13, bottom=0.18, right=0.95, top=0.80,
                wspace=0.2, hspace=0.2)
    ax1.set_ylim(0, 2.0)
    ax1.set_xlim(1, 19)
    plt.xticks([1, 3, 5, 7, 9, 11, 13, 15, 17, 19])

    legend1 = plt.legend((translation[0], segmentation[0], rate[0], runtime_parallel[0]), \
                        ('Loc. Error', 'Seg. Acc.', 'Compression Rate',  'FPS'), \
            bbox_to_anchor=(0., 1.01, 1., .101), loc=3,fontsize=12,
            ncol=4, mode="expand", borderaxespad=0., frameon=False)

    plt.gca().add_artist(legend1)
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor', direction="in")
    ax1.tick_params(axis='y',which='minor', direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("sensitivity.pdf");



def PlotRuntimeDesktop():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(7,3.5)).add_subplot(111)
    ax1.set_xlabel('Compression Rate', fontsize=16)
    ax1.set_ylabel('FPS', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)
    x_axis_ls = []

    real_time = ax1.plot([0.0, 120], [10.0, 10.0], color='red', linestyle='--',linewidth=1.5);
    # ax1.text(-14, 8, '10 FPS', fontsize=12, color='red',
    #         bbox=dict(boxstyle="square",
    #         ec=(1., 1., 1.), fc=(1., 1., 1.)))
                              
    pcc_stream = ax1.plot([21.81, 40.0, 55.26, 71.64, 88.89], \
                    [12.5, 28.57, 37.98, 52.08, 45.45],\
                    color='#71985E', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='^', markersize=12);

    pcc_frame = ax1.plot([19.91, 37.94, 51.83, 64.86, 82.76], \
                        [11.11, 23.81, 34.97, 47.17, 37.04], \
                        color='#FFBF56', linestyle='--', \
                        linewidth=1.5,markeredgecolor='k', marker='^', markersize=12);

    # img = ax1.plot([20.55], [3.95],\
    #                 color='#884dff', linestyle='--',\
    #                 linewidth=1.5,markeredgecolor='k', marker='o', markersize=12);

    video_lossy = ax1.plot([105.31], [73.53],\
                    color='#c90a0a', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='D', markersize=8);

    # plt.annotate('163.9', xy=(105, 90), xycoords='data',
    #          xytext=(106, 20), textcoords='data',fontsize=12,
    #          arrowprops=dict(arrowstyle="->"))


    vpcc = ax1.plot([112.43], [0.1],\
                    color='#b86207', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='s', markersize=10);

    video_lossless = ax1.plot([5.41], [50],\
                    color='#f5704e', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='D', markersize=9);

    octree = ax1.plot([8.45, 12.19, 15.31, 24.06, 53.33, 68.57], \
                    [1.21, 1.45, 1.93, 3.45, 3.80, 3.95], \
                    color='#5b87f2', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='X', markersize=12);

    # oct_n_trisoup = ax1.plot([4.85, 6.80, 11.60, 33.21], [0.185, 0.20, 0.256, 0.44], \
    #                 color='#16ADB0', linestyle='--',\
    #                 linewidth=1.5,markeredgecolor='k', marker='X', markersize=12);

    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    plt.yscale('log')
    plt.subplots_adjust(left=0.15, bottom=0.15, right=0.95, top=0.75,
                wspace=0.2, hspace=0.2)
    ax1.set_ylim(0.01, 100)
    ax1.set_xlim(0, 120)
    plt.yticks([0.01, 0.1, 1, 10, 100], ['0.01', '0.10', '1.0', '10.0', '100'])
    # plt.xticks([0, 5, 10, 15, 20, 25])

    plt.legend((pcc_frame[0], pcc_stream[0], video_lossy[0], 
                video_lossless[0], octree[0], vpcc[0]),
                ('Ours (single-frame)', 'Ours (streaming)', 'Video (lossy)', 
                 'Video (lossless)', 'GPCC',  'VPCC'),
                bbox_to_anchor=(0., 1.01, 1., .101), loc=3, fontsize=12,
            ncol=3, mode="expand", borderaxespad=0., frameon=False)
    
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("runtime_desktop.pdf");

def PlotRuntimeTX2():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(7,3.5)).add_subplot(111)
    ax1.set_xlabel('Compression Rate', fontsize=16)
    ax1.set_ylabel('FPS', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)

    real_time = ax1.plot([0.0, 120], [10.0, 10.0], color='red', linestyle='--',linewidth=1.5);
    # ax1.text(-14, 8, '10 FPS', fontsize=12, color='red',
    #         bbox=dict(boxstyle="square",
    #         ec=(1., 1., 1.), fc=(1., 1., 1.)))

    pcc_stream = ax1.plot([21.81, 40.0, 55.26, 71.64, 88.89], \
                    [7.94, 14.29, 18.01, 20.49, 18.94],\
                    color='#71985E', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='^', markersize=12);

    pcc_frame = ax1.plot([19.91, 37.94, 46.83, 64.86, 82.76], \
                        [7.5, 13.33, 16.30, 20.41, 18.18], \
                        color='#FFBF56', linestyle='--', \
                        linewidth=1.5,markeredgecolor='k', marker='^', markersize=12);

    video_lossy = ax1.plot([105.31], [19.23],\
                    color='#c90a0a', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='D', markersize=8);

    vpcc = ax1.plot([112.43], [0.0625],\
                    color='#b86207', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='s', markersize=10);

    video_lossless = ax1.plot([5.41], [16.67],\
                    color='#f5704e', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='D', markersize=9);

    octree = ax1.plot([8.45, 12.19, 15.31, 24.06, 68.57], \
                    [0.93, 1.05, 1.25, 1.78, 1.90], \
                    color='#5b87f2', linestyle='--',\
                    linewidth=1.5,markeredgecolor='k', marker='X', markersize=12);

    # oct_n_trisoup = ax1.plot([4.85, 6.80, 11.60, 33.21], [0.128, 0.14, 0.18, 0.30], \
    #                 color='#16ADB0', linestyle='--',\
    #                 linewidth=1.5,markeredgecolor='k', marker='X', markersize=12);
    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    plt.yscale('log')
    plt.subplots_adjust(left=0.15, bottom=0.15, right=0.95, top=0.75,
                wspace=0.2, hspace=0.2)
    ax1.set_ylim(0.01, 100)
    ax1.set_xlim(0, 120)
    plt.yticks([0.01, 0.1, 1, 10, 100], ['0.01', '0.10', '1.0', '10.0', '100'])
    # plt.xticks([0, 5, 10, 15, 20, 25])

    plt.legend((pcc_frame[0], pcc_stream[0], video_lossy[0],
                video_lossless[0], octree[0], vpcc[0]),
                ('Ours (single-frame)', 'Ours (streaming)', 'Video (lossy)', 
                 'Video (lossless)', 'GPCC',  'VPCC'),
                bbox_to_anchor=(0., 1.01, 1., .101), loc=3, fontsize=12,
            ncol=3, mode="expand", borderaxespad=0., frameon=False)
    
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("runtime_tx2.pdf");



# manually added the result and plot the three category.
def PlotTypeCategory():

    x_axis_ls  = [0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5]
    cross_fit  = [0.47, 0.46, 0.46, 0.44, 0.43, 0.41, 0.40, 0.39, 0.37]
    indiv_fit  = [0.30, 0.29, 0.30, 0.31, 0.31, 0.29, 0.32, 0.32, 0.29]
    median_fit = [0.05, 0.04, 0.05, 0.05, 0.06, 0.05, 0.06, 0.07, 0.05]
    unfit      = [0.18, 0.20, 0.19, 0.19, 0.20, 0.25, 0.21, 0.22, 0.28]


    Type_I = []
    Type_II = []
    Type_III = []
    Type_IV = []
    for i in range(len(cross_fit)):
        Type_I.append(cross_fit[i])
        Type_II.append(cross_fit[i]+indiv_fit[i])
        Type_III.append(cross_fit[i]+indiv_fit[i]+median_fit[i])
        Type_IV.append(1.0)


    plt.rc('font', size=12) # , weight='bold'
    ax = plt.figure(figsize=(6, 2.5)).add_subplot(111)
    ax.set_ylabel('Type Percentage', fontsize=12)
    plt.grid(color='grey', which='major', axis='y', linestyle='--')

    p4 = ax.bar(x_axis_ls, Type_IV, 0.5, align='center', color='#5b87f2',\
                edgecolor=['k']*len(x_axis_ls), linewidth=1.5, hatch="//");

    p3 = ax.bar(x_axis_ls, Type_III, 0.5, align='center', color='#FFBF56',\
                edgecolor=['k']*len(x_axis_ls), linewidth=1.5, hatch="oo");
    
    # p2 = ax.bar(x_axis_ls, Type_II, 0.5,align='center', color='#8154D1',\
    #             edgecolor=['k']*len(x_axis_ls), linewidth=1.5, hatch="||");
        
    p1 = ax.bar(x_axis_ls, Type_I, 0.5, align='center',color='#71985E',\
                edgecolor=['k']*len(x_axis_ls), linewidth=1.5, hatch="--");

    plt.subplots_adjust(left=0.14, bottom=0.10, right=0.97, top=0.80,
                wspace=0.2, hspace=0.2)
    ax.set_xticks([])
    # plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    ax.set_ylim(0.0, 1.0)
    ax.tick_params(axis="y",direction="in")
    ax.tick_params(axis="x",direction="in")
    plt.xticks(rotation=60)
    plt.setp(ax.spines.values(), linewidth=1.5)
    ax.yaxis.set_major_formatter(FuncFormatter(lambda y, _: '{:.0%}'.format(y))) 
    ax.set_axisbelow(True)

    plt.legend((p1[0], p3[0], p4[0]), ('Temporal', 'Spatial',\
                'Unfitting'), bbox_to_anchor=(0., 1.01, 1., .101), loc=3,
           ncol=3, mode="expand", borderaxespad=0., frameon=False)
    
    plt.savefig("type-category.pdf");


# manually added the result and plot the three category.
def PlotChannelSensitivity():

    plt.rc('font', size=12) # , weight='bold'
    ax = plt.figure(figsize=(6, 2.5)).add_subplot(111)
    ax.set_ylabel('Percentage (%)', fontsize=12)
    ax.set_xlabel('Number of Frames', fontsize=12)
    plt.grid(color='grey', which='major', axis='y', linestyle='--')


    p4 = ax.bar([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], \
                [100, 100, 100, 100, 100, 100, 100, 100, 100, 100], 1, align='center', color='#5b87f2',\
                edgecolor=['k']*10, linewidth=1.5, hatch="//");

    p3 = ax.bar([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], \
                [80.4, 30.6, 38.0, 42.0, 46.3, 47.3, 48.7, 50.8, 51.1, 53.1], 1, \
                bottom = [0.0, 49.8, 42.0, 38.0, 33.7, 30.7, 28.8, 29.2, 26.9, 26.5],
                align='center', color='#FFBF56',\
                edgecolor=['k']*10, linewidth=1.5, hatch="oo");
    
    # p2 = ax.bar(x_axis_ls, Type_II, 0.5,align='center', color='#8154D1',\
    #             edgecolor=['k']*len(x_axis_ls), linewidth=1.5, hatch="||");
        
    p1 = ax.bar([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], 
                [0.0, 49.8, 42.0, 38.0, 33.7, 30.7, 28.8, 29.2, 26.9, 26.5],
                 1, align='center',color='#71985E',\
                edgecolor=['k']*10, linewidth=1.5, hatch="--");

    plt.subplots_adjust(left=0.14, bottom=0.20, right=0.97, top=0.85,
                wspace=0.2, hspace=0.2)
    ax.set_xticks([1, 3, 5, 7, 9, 11, 13, 15, 17, 19])
    # plt.grid(color='grey', which='major', axis='y', linestyle='--')
    plt.grid(color='grey', which='major', axis='x', linestyle='--')
    ax.set_ylim(0.0, 100.0)
    ax.tick_params(axis="y",direction="in")
    ax.tick_params(axis="x",direction="in")
    # plt.xticks(rotation=60)
    plt.setp(ax.spines.values(), linewidth=1.5)
    # ax.yaxis.set_major_formatter(FuncFormatter(lambda y, _: '{:.0%}'.format(y))) 
    ax.set_axisbelow(True)

    plt.legend((p1[0], p3[0], p4[0]), ('Temporal', 'Spatial',\
                'Unfit'), bbox_to_anchor=(0., 1.01, 1., .101), loc=3,
           ncol=3, mode="expand", borderaxespad=0., frameon=False)
    
    
    plt.savefig("channel_sensitivity.pdf");


# manually added the result and plot the three category.
def PlotChannelSpeedup():
    plt.rc('font', size=12) # , weight='bold'
    ax = plt.figure(figsize=(6, 2)).add_subplot(111)
    ax.set_ylabel('Speedup', fontsize=12)
    ax.set_xlabel('Number of Frames', fontsize=12)

    parallel_speedup = ax.plot([1, 3, 5, 7, 9, 11, 13, 15, 17, 19], \
                [1, 3.69, 3.84, 4.33, 4.84, 4.89, 5.08, 5.23, 5.32, 5.44], \
                color='#884dff', linestyle='--',\
                linewidth=1.5, markeredgecolor='#884dff', \
                markerfacecolor='none', marker='v', markersize=12, clip_on=False);

    # absolute_speedup = ax.plot([1, 3, 5, 7, 10, 13, 16, 20], \
    #             [1, 3.83, 4.19, 5.01, 5.63, 5.94, 6.16, 6.41],\
    #             color='#71985E', linestyle='--',\
    #             linewidth=1.5,markeredgecolor='#71985E', \
    #             markerfacecolor='none',marker='^', markersize=12);

    plt.subplots_adjust(left=0.12, bottom=0.23, right=0.97, top=0.95,
                wspace=0.2, hspace=0.2)
    ax.set_xticks([0, 4, 8, 12, 16, 20])
    ax.set_ylim(0.0, 10.0)

    plt.minorticks_on()
    ax.tick_params(axis="y",direction="in")
    ax.tick_params(axis="x",direction="in")
    ax.tick_params(axis='x',which='minor', direction="in")
    ax.tick_params(axis='y',which='minor', direction="in")
    plt.setp(ax.spines.values(), linewidth=1.5)
    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    # plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # ax.yaxis.set_major_formatter(FuncFormatter(lambda y, _: '{:.0%}'.format(y))) 
    ax.set_axisbelow(True)
    ax.set_xlim(1, 19)
    plt.xticks([1, 3, 5, 7, 9, 11, 13, 15, 17, 19])

    # plt.legend(['Speedup'], \
    #         bbox_to_anchor=(0., 1.01, 1., .101), loc=3,
    #        ncol=3 , mode="expand", borderaxespad=0., frameon=False)
    
    plt.savefig("channel_speedup.pdf");

def PlotStream():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(5,4)).add_subplot(111)
    ax1.set_xlabel('Compression Configuration', fontsize=16)
    ax1.set_ylabel('Point-wise\nDistribution (%)', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)

    fitting_ratios = ax1.bar([0.5, 1.5, 2.5, 3.5, 4.5, 5.5], 
                            [72.3, 78, 72, 78.3, 69.7, 75.6], 0.8, \
                            edgecolor=['k']*5, align="center", \
                            color='#5b87f2', linewidth=1);

    fitting_tile_pct = ax1.bar([0.5, 1.5, 2.5, 3.5, 4.5, 5.5],
                    [55.4, 61.6, 54.3, 60.6, 50, 56], 0.8, \
                    edgecolor=['k']*5, align="center", \
                    color='#16ADB0', linewidth=1, hatch="//");

    fitting_merge_pct = ax1.bar([0.5, 1.5, 2.5, 3.5, 4.5, 5.5], \
                    [18.1, 27.7, 19.3, 31.4, 21.6, 32.8], 0.8, \
                    edgecolor=['k']*5, align="center", \
                    color='#71985E', linewidth=1, hatch="..");

    unfitting_tile_pct = ax1.bar([0.5, 1.5, 2.5, 3.5, 4.5, 5.5], \
                    [27.7, 22, 28, 21.7, 30.3, 24.4], \
                    bottom=[72.3, 78, 72, 78.3, 69.7, 75.6], width=0.8, \
                    edgecolor=['k']*5, align="center", \
                    color='#FFBF56', linewidth=1);

    unfitting_merge_pct = ax1.bar([0.5, 1.5, 2.5, 3.5, 4.5, 5.5], \
                    [6.9, 6.4, 6.2, 5.6, 6.0, 5.6], \
                    bottom=[72.3, 78, 72, 78.3, 69.7, 75.6], width=0.8, \
                    edgecolor=['k']*5, align="center", \
                    color='#8154D1', linewidth=1, hatch="\\\\");
    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    # plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # plt.xscale('log')
    plt.subplots_adjust(left=0.20, bottom=0.17, right=0.95, top=0.65,
                wspace=0.2, hspace=0.2)
    ax1.set_ylim(0, 100)
    ax1.set_xticklabels([])
    # ax1.set_xlim(-0.5, 10.5)
    # # plt.xticks([0, 5, 10, 15, 20, 25])
    # plt.xticks([2.5, 8.0, 13, 18], ['G-PCC', 'Ours(0.2)', 'Ours(0.5)', 'Ours(1.0)'])

    plt.legend((fitting_ratios[0], fitting_tile_pct[0], fitting_merge_pct[0],
                unfitting_tile_pct[0], unfitting_merge_pct[0]),
                ('Fitting\n(Tile-Non-Matched)', 'Fitting\n(Tile-Matched)','Fitting\n(Motion Compensated)', 
                 'Unfitting\n(Non-Compensated)', 'Unfitting\n(Motion Compensated)'), bbox_to_anchor=(0., 1.05, 1., .105), loc=4,
            ncol=2, borderaxespad=0., frameon=False, prop={'size': 10})
    
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("stream_compression.pdf");


def PlotStreamBpp():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(5,3)).add_subplot(111)
    ax1.set_xlabel('Compression Configuration', fontsize=16)
    ax1.set_ylabel('Bit-per-point (bpp)', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)

    fitting_bpp = ax1.bar([0.5, 1.5, 2.5, 3.5, 4.5, 5.5], 
                            [6.52, 5.57, 2.92, 2.39, 1.58, 1.31], 0.4, \
                            edgecolor=['k']*5, align="center", \
                            color='#5b87f2', linewidth=1, hatch="..");

    merge_bpp = ax1.bar([0.9, 1.9, 2.9, 3.9, 4.9, 5.9],
                    [6.17, 5.57, 2.74, 2.37, 1.46, 1.28], 0.4, \
                    edgecolor=['k']*5, align="center", \
                    color='#16ADB0', linewidth=1, hatch="//");
    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    # plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # plt.xscale('log')
    plt.subplots_adjust(left=0.20, bottom=0.17, right=0.95, top=0.85,
                wspace=0.2, hspace=0.2)
    ax1.set_ylim(0, 8)
    ax1.set_xticklabels([])
    # ax1.set_xlim(-0.5, 10.5)
    # # plt.xticks([0, 5, 10, 15, 20, 25])
    # plt.xticks([2.5, 8.0, 13, 18], ['G-PCC', 'Ours(0.2)', 'Ours(0.5)', 'Ours(1.0)'])

    plt.legend((fitting_bpp[0], merge_bpp[0]),
                ('Fitting', 'Streaming'), bbox_to_anchor=(0., 1.05, 1., .105), loc=3,
            ncol=2, borderaxespad=0., frameon=False, prop={'size': 10})
    
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("stream_bpp.pdf");


def PlotApplicationError():
    plt.rc('font', size=16)
    ax1 = plt.figure(figsize=(7,4)).add_subplot(111)
    ax1.set_xlabel('Compression Methods', fontsize=16)
    ax1.set_ylabel('Runtime (sec.)', fontsize=16)
    plt.setp(ax1.spines.values(), linewidth=1.5)

    regiestration = ax1.bar([0.5, 1.5, 2.5], 
                            [1.736, 2.212, 3.891], 0.8, \
                            edgecolor=['k']*5, align="center", \
                            color='#5b87f2', linewidth=1, hatch="//");

    detection = ax1.bar([4.5, 5.5, 6.5],
                    [0.184, 0.280, 0.388, 0.577, 0.672], 0.8, \
                    edgecolor=['k']*5, align="center", \
                    color='#16ADB0', linewidth=1, hatch="//");

    classification = ax1.bar([16.5, 17.5, 18.5, 19.5], \
                    [0.917, 3.224, 5.07, 6.88], 0.8, \
                    edgecolor=['k']*5, align="center", \
                    color='#71985E', linewidth=1, hatch="..");

    proj_n_fit_05 = ax1.bar([11.5, 12.5, 13.5, 14.5], \
                    [0.576, 1.91, 2.86, 3.84], 0.8, \
                    edgecolor=['k']*5, align="center", \
                    color='#FFBF56', linewidth=1, hatch="--");

    proj_n_fit_02 = ax1.bar([6.5, 7.5, 8.5, 9.5], \
                    [0.388, 1.05, 1.51, 2.05], 0.8, \
                    edgecolor=['k']*5, align="center", \
                    color='#8154D1', linewidth=1, hatch="\\\\");

    projection = ax1.bar([6.5, 7.5, 8.5, 9.5],
                    [0.0122, 0.0122, 0.0122, 0.0122], 0.8, \
                    edgecolor=['k']*5, align="center", \
                    color='tomato', linewidth=1, hatch="\\\\");



    plt.minorticks_on()

    plt.grid(color='grey', which='major', axis='y', linestyle='--')
    # plt.grid(color='grey', which='major', axis='x', linestyle='--')
    # plt.xscale('log')
    plt.subplots_adjust(left=0.10, bottom=0.17, right=0.95, top=0.93,
                wspace=0.2, hspace=0.2)
    ax1.set_ylim(0, 8)
    # ax1.set_xlim(-0.5, 10.5)
    # plt.xticks([0, 5, 10, 15, 20, 25])
    plt.xticks([2.5, 8.0, 13, 18], ['G-PCC', 'Ours(0.2)', 'Ours(0.5)', 'Ours(1.0)'])

    plt.legend((octree[0], oct_n_trisoup[0], projection[0],
                proj_n_fit_02[0], proj_n_fit_05[0], proj_n_fit_10[0]),
                ('Octree', 'Trisoup', '2D Projection',
                 'Fit 0.2', 'Fit 0.5', 'Fit 1.0'), ncol=3, prop={'size': 11})
    
    
    ax1.tick_params(axis="y",direction="in")
    ax1.tick_params(axis="x",direction="in")
    ax1.tick_params(axis='x',which='minor',direction="in")
    ax1.tick_params(axis='y',which='minor',direction="in")
    ax1.set_axisbelow(True)

    plt.savefig("compression_runtime.pdf");


PlotStreamBpp()
PlotStream()
PlotParentoByName()
PlotClusterResult()
PlotParentoTradeoff()
PlotDist2Approximation()
PlotStereoMismatch()
PlotRegiestration()
PlotRuntimeDesktop()
PlotRuntimeTX2()
PlotDetection()
PlotSegmentation()
PlotSensitivity()
PlotTypeCategory()
PlotChannelSensitivity()
PlotChannelSpeedup()

# def PlotDetection():
#     plt.rc('font', size=14)
#     ax1 = plt.figure(figsize=(7,3.5)).add_subplot(111)
#     ax1.set_xlabel('Compression Rate', fontsize=16)
#     ax2 = ax1.twinx()
#     ax1.set_ylabel('Compression Rate', fontsize=16)
#     ax2.set_ylabel('BEV Accuracy (%)', fontsize=16)
#     plt.setp(ax1.spines.values(), linewidth=1.5)

#     x_axis_ls = []
                              
#     org_pipe = ax1.bar([1], [1], \
#                         edgecolor=['k'], align="center", \
#                         color='#5b87f2', linewidth=1, hatch="//")

#     pcc_opt = ax1.bar([3, 4, 5, 6],  [11.47, 24.43, 42.29, 57.15], \
#                     edgecolor=['k']*3, align="center", \
#                     color='#71985E', linewidth=1, hatch="//")

#     org_pipe_c = ax2.plot([1], [74.46],  color='#5b87f2', linestyle='none', \
#                     linewidth=1.5, markeredgecolor='k', marker='o', markersize=16);

#     pcc_opt_c = ax2.plot([3, 4, 5, 6], [72.15, 70.75, 69.55, 65.85], \
#                     color='#71985E', linestyle='none',\
#                     linewidth=1.5,markeredgecolor='k', marker='v', markersize=16)

#     octree = ax1.bar([8, 9, 10], [8.45, 15.31, 33.21],\
#                     edgecolor=['k']*2, align="center", \
#                     color='#FFBF56', linewidth=1, hatch="--")

#     octree_c = ax2.plot([8, 9, 10], [42.12, 41.94, 39.42], color='#FFBF56', linestyle='none',\
#                     linewidth=1.5,markeredgecolor='k', marker='^', markersize=16);

#     jpeg = ax1.bar([12, 13, 14], [20.55, 25.19, 33.45],\
#                     edgecolor=['k']*2, align="center", \
#                     color='#884dff', linewidth=1, hatch="--")

#     jpeg_c = ax2.plot([12, 13, 14], [57.56, 43.29, 14.15], color='#884dff', linestyle='none',\
#                     linewidth=1.5,markeredgecolor='k', marker='D', markersize=14);

#     plt.minorticks_on()

#     plt.grid(color='grey', which='major', axis='y', linestyle='--')
#     plt.grid(color='grey', which='major', axis='x', linestyle='--')
#     # plt.yscale('log')
#     plt.subplots_adjust(left=0.15, bottom=0.14, right=0.85, top=0.65,
#                 wspace=0.2, hspace=0.2)
#     ax1.set_ylim(0, 100)
#     ax2.set_ylim(0, 100)
#     # ax1.set_xlim(0, 100)
#     # plt.xticks([0, 5, 10, 15, 20, 25])
#     plt.xticks([1, 4.5, 9, 12.5], ['Org.', 'Ours', 'G-PCC', 'JPEG'])

#     legend1 = plt.legend((org_pipe[0], pcc_opt[0], octree[0], jpeg[0]), \
#                 ('Org. Pipeline', 'Ours','G-PCC', 'JPEG'), \
#             bbox_to_anchor=(0., 1.01, 1., .101), loc=3,
#             ncol=1, mode="expand", borderaxespad=0., frameon=False)

#     legend2 = plt.legend((org_pipe_c[0], pcc_opt_c[0], octree_c[0], jpeg_c[0]), \
#                 ('Org. Pipeline', 'Ours','G-PCC', 'JPEG'), \
#             bbox_to_anchor=(0., 1.01, 1., .101), loc=4,
#             ncol=1, borderaxespad=0., frameon=False)

#     plt.gca().add_artist(legend1)
#     plt.gca().add_artist(legend2)
    
#     ax1.tick_params(axis="y",direction="in")
#     ax1.tick_params(axis="x",direction="in")
#     ax1.tick_params(axis='x',which='minor',direction="in")
#     ax1.tick_params(axis='y',which='minor',direction="in")
#     ax1.set_axisbelow(True)

#     plt.savefig("detection.pdf");

# def PlotSegmentation():
#     plt.rc('font', size=14)
#     ax1 = plt.figure(figsize=(7,3.5)).add_subplot(111)
#     ax1.set_xlabel('Compression Rate', fontsize=16)
#     ax2 = ax1.twinx()
#     ax1.set_ylabel('Compression Rate', fontsize=16)
#     ax2.set_ylabel('Seg. Accuracy (%)', fontsize=16)
#     plt.setp(ax1.spines.values(), linewidth=1.5)

#     x_axis_ls = []
                              
#     org_pipe = ax1.bar([1], [1], \
#                         edgecolor=['k'], align="center", \
#                         color='#5b87f2', linewidth=1, hatch="//")

#     org_pipe_c = ax2.plot([1], [76.2],  color='#5b87f2', linestyle='none', \
#                     linewidth=1.5, markeredgecolor='k', marker='o', markersize=16);

#     pcc_opt = ax1.bar([3, 4, 5, 6, 7, 8],  [11.47, 15.41, 24.43, 31.27, 42.30, 57.14], \
#                     edgecolor=['k']*3, align="center", \
#                     color='#71985E', linewidth=1, hatch="//")

#     pcc_opt_c = ax2.plot([3, 4, 5, 6, 7, 8], [75.5, 74.1, 73.6, 72.4, 39.3, 38.1], \
#                     color='#71985E', linestyle='none',\
#                     linewidth=1.5,markeredgecolor='k', marker='v', markersize=16)

#     octree = ax1.bar([10, 11, 12, 13], [8.45, 12.20, 15.31, 33.21],\
#                     edgecolor=['k']*2, align="center", \
#                     color='#FFBF56', linewidth=1, hatch="--")

#     octree_c = ax2.plot([10, 11, 12, 13], [75.2, 70, 51.8, 33.9], color='#FFBF56', linestyle='none',\
#                     linewidth=1.5,markeredgecolor='k', marker='^', markersize=16)

#     jpeg = ax1.bar([15, 16, 17], [20.55, 25.19, 33.45],\
#                     edgecolor=['k']*2, align="center", \
#                     color='#884dff', linewidth=1, hatch="--")

#     jpeg_c = ax2.plot([15, 16, 17], [71.5, 69.2, 66.8], \
#                     color='#884dff', linestyle='none',\
#                     linewidth=1.5,markeredgecolor='k', marker='D', markersize=14)


#     plt.minorticks_on()

#     plt.grid(color='grey', which='major', axis='y', linestyle='--')
#     plt.grid(color='grey', which='major', axis='x', linestyle='--')
#     # plt.yscale('log')
#     plt.subplots_adjust(left=0.15, bottom=0.14, right=0.85, top=0.65,
#                 wspace=0.2, hspace=0.2)
#     ax1.set_ylim(0, 100)
#     ax2.set_ylim(30, 80)
#     # ax1.set_xlim(0, 100)
#     # plt.xticks([0, 5, 10, 15, 20, 25])
#     plt.xticks([1, 5.5, 11.5, 16], ['Org-Pipeline', 'Ours', 'G-PCC', 'JPEG'])

#     legend1 = plt.legend((org_pipe[0], pcc_opt[0], octree[0], jpeg[0]), \
#                 ('Org. Pipeline', 'Ours','G-PCC', 'JPEG'), \
#             bbox_to_anchor=(0., 1.01, 1., .101), loc=3,
#             ncol=1, mode="expand", borderaxespad=0., frameon=False)

#     legend2 = plt.legend((org_pipe_c[0], pcc_opt_c[0], octree_c[0], jpeg_c[0]), \
#                 ('Org. Pipeline', 'Ours','G-PCC', 'JPEG'), \
#             bbox_to_anchor=(0., 1.01, 1., .101), loc=4,
#             ncol=1, borderaxespad=0., frameon=False)

#     plt.gca().add_artist(legend1)
#     plt.gca().add_artist(legend2)
    
#     ax1.tick_params(axis="y",direction="in")
#     ax1.tick_params(axis="x",direction="in")
#     ax1.tick_params(axis='x',which='minor',direction="in")
#     ax1.tick_params(axis='y',which='minor',direction="in")
#     ax1.set_axisbelow(True)

#     plt.savefig("segmentation.pdf");


# def PlotRuntime():
#     plt.rc('font', size=16)
#     ax1 = plt.figure(figsize=(7,4)).add_subplot(111)
#     ax1.set_xlabel('Compression Methods', fontsize=16)
#     ax1.set_ylabel('Runtime (sec.)', fontsize=16)
#     plt.setp(ax1.spines.values(), linewidth=1.5)

#     oct_n_trisoup = ax1.bar([0.5, 1.5, 2.5, 3.5, 4.5], 
#                             [1.736, 2.212, 3.891, 4.844, 5.421], 0.8, \
#                             edgecolor=['k']*5, align="center", \
#                             color='#5b87f2', linewidth=1, hatch="//");

#     octree = ax1.bar([0.5, 1.5, 2.5, 3.5, 4.5],
#                     [0.184, 0.280, 0.388, 0.577, 0.672], 0.8, \
#                     edgecolor=['k']*5, align="center", \
#                     color='#16ADB0', linewidth=1, hatch="//");

#     proj_n_fit_10 = ax1.bar([16.5, 17.5, 18.5, 19.5], \
#                     [0.917, 3.224, 5.07, 6.88], 0.8, \
#                     edgecolor=['k']*5, align="center", \
#                     color='#71985E', linewidth=1, hatch="..");

#     proj_n_fit_05 = ax1.bar([11.5, 12.5, 13.5, 14.5], \
#                     [0.576, 1.91, 2.86, 3.84], 0.8, \
#                     edgecolor=['k']*5, align="center", \
#                     color='#FFBF56', linewidth=1, hatch="--");

#     proj_n_fit_02 = ax1.bar([6.5, 7.5, 8.5, 9.5], \
#                     [0.388, 1.05, 1.51, 2.05], 0.8, \
#                     edgecolor=['k']*5, align="center", \
#                     color='#8154D1', linewidth=1, hatch="\\\\");

#     projection = ax1.bar([6.5, 7.5, 8.5, 9.5],
#                     [0.0122, 0.0122, 0.0122, 0.0122], 0.8, \
#                     edgecolor=['k']*5, align="center", \
#                     color='tomato', linewidth=1, hatch="\\\\");

#     plt.minorticks_on()

#     plt.grid(color='grey', which='major', axis='y', linestyle='--')
#     # plt.grid(color='grey', which='major', axis='x', linestyle='--')
#     # plt.xscale('log')
#     plt.subplots_adjust(left=0.10, bottom=0.17, right=0.95, top=0.93,
#                 wspace=0.2, hspace=0.2)
#     ax1.set_ylim(0, 8)
#     # ax1.set_xlim(-0.5, 10.5)
#     # plt.xticks([0, 5, 10, 15, 20, 25])
#     plt.xticks([2.5, 8.0, 13, 18], ['G-PCC', 'Ours(0.2)', 'Ours(0.5)', 'Ours(1.0)'])

#     plt.legend((octree[0], oct_n_trisoup[0], projection[0],
#                 proj_n_fit_02[0], proj_n_fit_05[0], proj_n_fit_10[0]),
#                 ('Octree', 'Trisoup', '2D Projection',
#                  'Fit 0.2', 'Fit 0.5', 'Fit 1.0'), ncol=3, prop={'size': 11})
    
    
#     ax1.tick_params(axis="y",direction="in")
#     ax1.tick_params(axis="x",direction="in")
#     ax1.tick_params(axis='x',which='minor',direction="in")
#     ax1.tick_params(axis='y',which='minor',direction="in")
#     ax1.set_axisbelow(True)

#     plt.savefig("compression_runtime.pdf");


# # manually added the result and plot the three category.
# def PlotChannelSensitivity():
#     plt.rc('font', size=12) # , weight='bold'
#     ax = plt.figure(figsize=(6, 2.5)).add_subplot(111)
#     ax.set_ylabel('Type Percentage (%)', fontsize=12)
#     ax.set_xlabel('Number of Frame', fontsize=12)

#     cross_fit = ax.plot([2, 3, 5, 7, 10, 15, 20], \
#                 [56.3, 49.8, 42.0, 38.0, 33.7, 29.2, 26.9], \
#                 color='#884dff', linestyle='--',\
#                 linewidth=1.5, markeredgecolor='#884dff', \
#                 markerfacecolor='none', marker='v', markersize=12, clip_on=False);

#     indiv_fit = ax.plot([2, 3, 5, 7, 10, 15, 20], \
#                 [23.9, 30.6, 38.0, 42.0, 46.3, 50.8, 53.1],\
#                 color='#71985E', linestyle='--',\
#                 linewidth=1.5,markeredgecolor='#71985E', \
#                 markerfacecolor='none',marker='^', markersize=12);

#     # median_fit = ax.plot([2, 3, 5, 7, 10, 15, 20], \
#     #             [4.9, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0],\
#     #             color='#b86207', linestyle='--',\
#     #             linewidth=1.5,markeredgecolor='#b86207', \
#     #             markerfacecolor='none',marker='X', markersize=12);

#     plt.subplots_adjust(left=0.14, bottom=0.20, right=0.97, top=0.83,
#                 wspace=0.2, hspace=0.2)
#     ax.set_xticks([0, 4, 8, 12, 16, 20])
#     ax.set_ylim(0.0, 100.0)

#     plt.minorticks_on()
#     ax.tick_params(axis="y",direction="in")
#     ax.tick_params(axis="x",direction="in")
#     ax.tick_params(axis='x',which='minor', direction="in")
#     ax.tick_params(axis='y',which='minor', direction="in")
#     plt.setp(ax.spines.values(), linewidth=1.5)
#     plt.grid(color='grey', which='major', axis='y', linestyle='--')
#     # plt.grid(color='grey', which='major', axis='x', linestyle='--')
#     # ax.yaxis.set_major_formatter(FuncFormatter(lambda y, _: '{:.0%}'.format(y))) 
#     ax.set_axisbelow(True)

#     plt.legend((cross_fit[0], indiv_fit[0]), \
#             ('Temporal', 'Spatial'), \
#             bbox_to_anchor=(0., 1.01, 1., .101), loc=3,
#            ncol=3 , mode="expand", borderaxespad=0., frameon=False)
    
#     plt.savefig("channel_sensitivity.pdf");

