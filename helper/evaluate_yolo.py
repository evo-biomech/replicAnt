import sys
from sklearn.metrics import confusion_matrix, plot_confusion_matrix
import matplotlib.pyplot as plt
from operator import itemgetter
import numpy as np

# load darknet with compiled DLLs for windows from respective path
sys.path.append('I:\\Antrax\darknet-master\\build\darknet\\x64')
import darknet

# load images and labels for evaluation
absolute_path = 'I:\\Antrax\\AntraxV2\\trained_real_data_corrected\\'
test_images_file_path = 'I:\\Antrax\\AntraxV2\\trained_real_data_corrected\\data\\test.txt'
test_images_paths = []
test_labels = []

print('Loading image and label paths ...')


def nonMaximumSupression(detections):
    """
    :param detections: detections returned from darknet
    :return: only detection of highest confidence. Return None, if no individual was detected
    """
    if len(detections) != 0:
        det_sorted = sorted(detections, key=itemgetter(2))
        max_conf_detection = det_sorted[0][0]
    else:
        max_conf_detection = 'No Detect'
    return max_conf_detection


def computeAbsPercentageError(true, predicted):
    """
    :param true: ground truth value
    :param predicted: predicted value
    :return: absolute percentage error
    """
    return abs((true - predicted) / true) * 100


with open(test_images_file_path) as f:
    for line in f:
        # create absolute paths
        test_images_paths.append(absolute_path + line)
        # use windows notation of file paths
        test_images_paths[-1] = test_images_paths[-1].replace('/', '\\')
        # remove line break at the end of each string
        test_images_paths[-1] = test_images_paths[-1][:-1]

        # import labels by using the corresponding .txt file for each image
        test_label_path = test_images_paths[-1][:-3] + 'txt'
        with open(test_label_path) as t:
            label_file_content = []
            for line_txt in t:
                label_file_content.append(line_txt.split(' '))
            # remove line break at the end of each string
            label_file_content[-1] = label_file_content[-1][:-1]

            # the label of the image is the first entry stored in the label file
            test_labels.append(label_file_content)

# load configuration and weights
yolov3_cfg = 'I:\\Antrax\\AntraxV2\\synthetic_data_medium_only_second\\cfg\\yolov3_ant_weight_testing.cfg'
yolov3_weights = 'I:\\Antrax\\AntraxV2\\synthetic_data_medium_only_second\\yolov3_ant_weight_final_PLUS_SYNTH.weights'
yolov3_data = 'I:\\Antrax\\AntraxV2\\synthetic_data_medium_only_second\\data\\obj.data'
yolov3_names = 'I:\\Antrax\\AntraxV2\\synthetic_data_medium_only_second\\data\\obj.names'

# translate labels into object names
actual_img_labels = []
actual_names = []
actual_weight = []

# compute the MAPE
ideal_APE = []  # given the class is correct (deviation from true value when grouped into classes)
class_APE = []  # deviation between prediction and class, assuming discrete weight classes.
actual_APE = []  # actual deviation from prediction to true value

# load names from obj.names
with open(yolov3_names) as names:
    for name in names:
        actual_names.append(name[:-1])

# now assign those names back to each labelled test image
# TODO add support for multiple individuals per image
for img in test_labels:
    actual_img_labels.append(actual_names[int(img[0][0])])
# add "No detect" as possible label to the list of names
actual_names.append('No Detect')

print(actual_names)

print("Found", len(test_images_paths), "labelled test images!")

# view docstring of performDetect for details
# run ones to initialise darknet with initOnly=True for faster inference without reloading afterwards
darknet.performDetect(imagePath=test_images_paths[-1], thresh=0.25, configPath=yolov3_cfg,
                      weightPath=yolov3_weights, metaPath=yolov3_data, showImage=False, makeImageOnly=False,
                      initOnly=True)

tested_images = 0
predicted_img_labels = []

for img in test_images_paths:
    print("Using:", img)
    actual_weight.append(img[-10:-4])  # actual weight, as measured
    print("With a measured weight of:", actual_weight[-1])
    detections = darknet.performDetect(imagePath=img, thresh=0.25, configPath=yolov3_cfg,
                                       weightPath=yolov3_weights, metaPath=yolov3_data, showImage=False,
                                       makeImageOnly=False,
                                       initOnly=False)
    predicted_img_labels.append(nonMaximumSupression(detections))
    print("Predicted weight:\n", predicted_img_labels[-1], "\nActual weight:", actual_img_labels[tested_images], "g")

    if predicted_img_labels[-1] != 'No Detect':
        # compute Absolute Percentage Errors, excluding failure to detect
        ideal_APE.append(computeAbsPercentageError(true=float(actual_weight[-1]),
                                                   predicted=float(actual_img_labels[tested_images])))
        class_APE.append(computeAbsPercentageError(true=float(actual_img_labels[tested_images]),
                                                   predicted=float(predicted_img_labels[-1])))
        actual_APE.append(computeAbsPercentageError(true=float(actual_weight[-1]),
                                                    predicted=float(predicted_img_labels[-1])))

    tested_images += 1

"""
Compute the ideal, class, and actual MAPE
"""
ideal_MAPE = np.round(np.mean(ideal_APE), 2)
class_MAPE = np.round(np.mean(class_APE), 2)
actual_MAPE = np.round(np.mean(actual_APE), 2)

# compute MAPE for sampled class only

print("\nThe ideal MAPE for the given dataset and class width is", ideal_MAPE, " %")
print("The class MAPE, assuming the class label to be the ground truth, is", class_MAPE, " %")
print("The actual MAPE, compared to the measured ground truth is", actual_MAPE, " %\n")

conf_mat = confusion_matrix(actual_img_labels, predicted_img_labels, labels=actual_names)
print(conf_mat)

fig, ax = plt.subplots()
im = ax.imshow(conf_mat, cmap=plt.cm.cividis)
# further colour maps:
# https://matplotlib.org/3.1.0/tutorials/colors/colormaps.html

# show all ticks and label them with the respective list entries
ax.set_xticks(np.arange(len(actual_names)))
ax.set_yticks(np.arange(len(actual_names)))

ax.set_xticklabels(actual_names)
ax.set_yticklabels(actual_names)

ax.set_ylabel('true label')
ax.set_xlabel('predicted label')

# Rotate the tick labels and set their alignment.
plt.setp(ax.get_xticklabels(), rotation=45, ha="right",
         rotation_mode="anchor")

# Loop over data dimensions and create text annotations.
for i in range(len(actual_names)):
    for j in range(len(actual_names)):
        text = ax.text(j, i, conf_mat[i, j],
                       ha="center", va="center", color="w")

ax.set_title("Confusion Matrix: discretised weight estimation")
# fig.tight_layout()
plt.show()
