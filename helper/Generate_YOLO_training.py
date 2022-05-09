import numpy as np
import cv2
import csv
import os
import math
import time
import random
from sklearn.utils import shuffle
from sklearn.model_selection import KFold
from imutils import paths
from operator import itemgetter

def import_tracks(path, numFrames, export=False):
    """
    Import all tracked paths (using blender motionExport.py) from specified folder and join them to a single array.
    Optionally, allows for export of created array containing all tracks into single .csv file

    :param path: location of exported .csv tracks
    :param numFrames: number of total analysed frames
    :param export: boolean, writes .csv file of all combined tracks if True

    :return: array of all imported tracks, row: frames, columns X / Y coordinates of individual track.
             The first column consists of the frame numbers for easier readability if exported as a single file.
    """
    print("importing tracks...")
    files = []
    tracks = np.empty([numFrames + 1, 1])  # create array for all tracks
    tracks[:, 0] = np.arange(start=1, stop=numFrames + 2, step=1, dtype=int)  # insert frame numbers

    imported = 0

    # r=root, d=directories, f = files
    for r, d, f in os.walk(path):
        for file in f:
            if '.csv' in file:
                files.append(os.path.join(r, file))

                # for each new track create two "zeros" columns
                # zeros are handled as nonexistent instances
                tracks = np.append(tracks, np.zeros([numFrames + 1, 2]), axis=1)

                with open(files[imported]) as csv_file:
                    csv_reader = csv.reader(csv_file, delimiter=';')
                    line_count = 0

                    next(csv_reader, None)  # skip the headers

                    for row in csv_reader:
                        # tracks.insert())
                        tracks[int(row[0]) - 1, imported * 2 + 1] = int(row[1])
                        tracks[int(row[0]) - 1, imported * 2 + 2] = int(row[2])
                        line_count += 1
                    print("imported", str(file), f' with {line_count} points.')

                imported += 1

    tracks = tracks.astype(int)
    if export:
        np.savetxt("all_tracks.csv", tracks, delimiter=",")

    print("\nSuccessfully combined the tracks of", imported, "individuals for training and display!")
    return tracks


def display_video(cap, tracks, show=(0, math.inf), scale=1.0):
    """
    Function displays imported footage with tracking results as overlay

    :param cap: Imported video file
    :param tracks: all imported tracks as a single array, created with import_tracks
    :param show: tuple of desired displayed frames
    :param scale: single float to up- or downscale resolution of display
    """
    tracks = (scale * tracks).astype(int)  # rescale pixel values of tracks
    # frame counter
    frame_num = show[0]

    # define the size of each tracking rectangle
    target_size = 100 * scale

    # get frame rate of imported footage
    fps = cap.get(cv2.CAP_PROP_FPS)

    # fix the seed for the same set of randomly assigned colours for each track
    np.random.seed(seed=0)
    colours = np.random.randint(low=0, high=255, size=((math.floor(((tracks.shape[1]) - 1) / 2)), 3))

    print("\nDisplaying tracked footage!\npress 'q' to end display")

    # skip to desired start frame
    # Property identifier of cv2.CV_CAP_PROP_POS_FRAMES is 1, thus the first entry is 1
    cap.set(1, show[0])

    # set font from info display on frame
    font = cv2.FONT_HERSHEY_SIMPLEX

    while True:  # run until no more frames are available
        time_prev = time.time()
        # return single frame (ret = boolean, frame = image)
        ret, frame = cap.read()
        if not ret:
            break

        # scale down the video
        new_height = int(np.shape(frame)[0] * scale)
        new_width = int(np.shape(frame)[1] * scale)
        frame = cv2.resize(frame, (new_width, new_height))

        # iterate through all columns and draw rectangles for all non 0 values
        for track in range(math.floor(((tracks.shape[1]) - 1) / 2)):
            if tracks[frame_num, track * 2 + 1] != 0:
                # the tracks are read as centres
                target_centre = np.asarray([tracks[frame_num, track * 2 + 1], tracks[frame_num, track * 2 + 2]])

                # invert y axis, to fit openCV convention ( lower left -> (x=0,y=0) )
                target_centre[1] = new_height - target_centre[1]
                # define the starting and ending point of the bounding box rectangle, defined by "target_size"
                px_start = target_centre - np.asarray([math.floor(target_size / 2), math.floor(target_size / 2)])
                px_end = target_centre + np.asarray([math.floor(target_size / 2), math.floor(target_size / 2)])
                # draw the defined rectangle of the track on top of the frame
                cv2.rectangle(frame, (px_start[0], px_start[1]), (px_end[0], px_end[1]),
                              (int(colours[track, 0]), int(colours[track, 1]), int(colours[track, 2])), 2)
                # write out track number of each active track
                cv2.putText(frame, "track: " + str(track),
                            (int(target_centre[0] - target_size / 2), int(target_centre[1] - target_size / 2 - 10)),
                            font, 0.3, (int(colours[track, 0]), int(colours[track, 1]), int(colours[track, 2])), 1,
                            cv2.LINE_AA)

        cv2.putText(frame, "frame: " + str(frame_num), (int(new_width / 2) - 100, 35),
                    font, 0.8, (255, 255, 255), 1, cv2.LINE_AA)
        cv2.imshow('original frame', frame)

        if frame_num > show[1]:
            break

        # enforce constant frame rate during display
        time_to_process = (time.time() - time_prev)  # compute elapsed time to enforce constant frame rate (if possible)
        if time_to_process < 1 / fps:
            time.sleep((1 / fps) - time_to_process)

        # press q to quit, i.e. exit the display
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

        frame_num += 1

    cv2.destroyAllWindows()

    # always reset frame from capture at the end to avoid incorrect skips during access
    cap.set(1, 0)

    print("\nReached last frame of specified video or ended by user input.\n")


def video_to_dataset(video, tracks, ROI, use_frames=(0, math.inf), output_path="", dataset_img_name="Img", target_size=100,
                     known_weights=None):
    """
    Exports individual frames and corresponding labels from imported tracks
    :param video: input video file (must be readable by OpenCV)
    :param tracks: format specified by import_tracks function
    :param ROI: region of interest in imported video file
    :param use_frames: range of frames to be exported
    :param target_size: target size in px
    """

    # skip to desired start frame
    # Property identifier of cv2.CV_CAP_PROP_POS_FRAMES is 1, thus the first entry is 1
    video.set(1, use_frames[0])

    # create new folders for the exported images and tracks
    output_path = output_path + "/data"
    if not os.path.exists(output_path):
        os.mkdir(output_path)
    output_path = output_path + "/obj"
    if not os.path.exists(output_path):
        os.mkdir(output_path)

    frame_width = ROI[1][1] - ROI[1][0]
    frame_height = ROI[0][1] - ROI[0][0]

    orig_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))

    print("Cropped Width =", frame_width)
    print("Cropped Height =", frame_height)

    # begin at first selected frame
    frame_num = use_frames[0]

    # fix the seed for the same set of randomly assigned colours for each track
    np.random.seed(seed=0)
    colours = np.random.randint(low=0, high=255, size=((math.floor(((tracks.shape[1]) - 1) / 2)), 3))

    while True:  # run until no more frames are available

        # return single frame (ret = boolean, frame = image)
        ret, frame = video.read()
        if not ret:
            break

        # crop the video ROI[ y_low : y_high, x_low : x_high ]
        frame = frame[ROI[0][0]:ROI[0][1], ROI[1][0]:ROI[1][1]]

        # save the cropped frame to the respective folder
        cv2.imwrite(output_path + "/" + dataset_img_name + "_frame_" + str(frame_num) + ".JPG", frame)

        with open(output_path + "/" + dataset_img_name + "_frame_" + str(frame_num) + ".txt", "w") as f:
            # iterate through all columns and draw rectangles for all non 0 values
            for track in range(math.floor(((tracks.shape[1]) - 1) / 2)):
                if tracks[frame_num, track * 2 + 1] != 0:
                    # the tracks are read as centres
                    target_centre = np.asarray([tracks[frame_num, track * 2 + 1], tracks[frame_num, track * 2 + 2]])

                    target_centre_adj = target_centre - [ROI[1][0], orig_height - ROI[0][1]]

                    # invert y axis, to fit OpenCV convention:
                    target_centre_adj[1] = frame.shape[0] - target_centre_adj[1]

                    """
                    (0,0) for:
                    
                    OpenCV:                         upper left
                    darknet:                        upper left
                    literally everything else :     upper left
                    Blender:                        lower left (thanks, Blender)
                    """

                    line = "0 " + str(target_centre_adj[0] / frame_width) + " " \
                           + str(target_centre_adj[1] / frame_height) + " " \
                           + str(target_size / frame_width) + " " + str(target_size / frame_height) + "\n"

                    f.write(line)

                    """
                    test whether cropping and labelling went correctly
                    """

                    px_start = target_centre_adj - np.asarray(
                        [math.floor(target_size / 2), math.floor(target_size / 2)])
                    px_end = target_centre_adj + np.asarray([math.floor(target_size / 2), math.floor(target_size / 2)])
                    # draw the defined rectangle of the track on top of the frame
                    cv2.rectangle(frame, (px_start[0], px_start[1]), (px_end[0], px_end[1]),
                                  (int(colours[track, 0]), int(colours[track, 1]), int(colours[track, 2])), 2)

                cv2.imshow('cropped frame', frame)

        if frame_num > use_frames[1]:
            break

        # press q to quit, i.e. exit the display
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

        frame_num += 1

    cv2.destroyAllWindows()

    # always reset frame from capture at the end to avoid incorrect skips during access
    video.set(1, 0)


def image_patch_to_dataset(input_paths):
    """
    generate data set from input images in path locations
    :param input_paths: list of all paths where images are located for the datas et generation
    :return: label files for each imported image
    """
    # create new folders for the exported images and tracks
    path_images = "obj"

    if not os.path.exists(path_images):
        os.mkdir(path_images)

    # assign the size of the used detection
    px_area = 100

    # using 20 classes of discretized mass
    num_classes = 20
    lowest_weight = 0.0001
    highest_weight = 0.025
    range_weight = highest_weight - lowest_weight
    class_width = range_weight / num_classes

    classes = np.arange(lowest_weight, highest_weight, class_width, dtype=float)
    classes = np.around(classes, decimals=4)
    print("Grouped into", num_classes, "classes with a width of", class_width, "g")
    print("Using the following class centres:\n", classes)

    weights = []
    for path in input_paths:
        for imagePath in sorted(paths.list_images(path)):
            print("importing:", imagePath)
            # read weight from image path
            weights.append(float(imagePath[-10:-4]))
            print("with mass:", weights[-1], "g")

            # the default label is the highest class, so anything smaller can be grouped accordingly
            label = num_classes - 1

            for i in range(num_classes):
                # check whether the individual is within the range of a class and assign a label accordingly
                # the range is defined by the centre of the class and the width
                if weights[-1] < classes[i] - class_width / 2:
                    print("weight class:", classes[i - 1], "g")
                    label = i - 1
                    print("assigned label:", label)
                    break

            # as these images all have the animal in its centre, the bounding box will always be 100 x 100 px with the
            # centre located in the centre of the image.

            # save the current image to the respective training directory
            img = cv2.imread(imagePath)
            file_name = imagePath.split("\\")[-1]
            cv2.imwrite(path_images + "\\" + file_name, img)

            with open(path_images + "\\" + file_name[:-4] + ".txt", "w") as f:
                # get image centre
                centre = (1 / 2, 1 / 2)  # centre is always to set in the middle of the image
                img_rel_width = px_area / img.shape[0]
                img_rel_height = px_area / img.shape[1]

                line = str(label) + " " + str(centre[0]) + " " + str(centre[1]) + " " + str(img_rel_width) + " " + str(
                    img_rel_height) + "\n"

                f.write(line)

    return classes


def createCustomFiles(obIDs=["Ant"], amountTest=0.1, random_state=0, k_fold=[5,None], output_folder="", custom_name=None):
    """
    Creates custom folder and files for training and testing YOLOv3 & YOLOv4 with Darknet Framework
    :param obIDs:List of names of objects
    :param amountTest: amount of testing data to be withheld from training
    :param output_folder: specify output folder if desired
    """
    if custom_name is not None:
        paths_folders = [output_folder + "data", output_folder + "data/obj", output_folder + "data"] * 3
        train_file = "data/" + custom_name +"_train.txt"
        test_file = "data/" + custom_name +"_test.txt"
        names_file = "data/obj.names"
    else:
        paths_folders = [output_folder + "data", output_folder + "data/obj", output_folder + "data"] * 3
        train_file = "data/train.txt"
        test_file = "data/test.txt"
        names_file = "data/obj.names"

    if len(obIDs) != 1:
        print("Using custom labels:", obIDs)

    # create all required folders
    for folder in range(len(paths_folders)):
        if not os.path.exists(paths_folders[folder]):
            os.mkdir(paths_folders[folder])

    # create object file (contains list of names, corresponding to objectIDs)
    with open(paths_folders[2] + "/" + "obj.names", "w") as f:
        for ob in range(len(obIDs)):
            f.write(str(obIDs[ob]) + "\n")

    # create trainer.data fle based on inputs
    if custom_name is not None:
        with open(paths_folders[2] + "/" + custom_name, "w") as f:
            f.write("classes = " + str(len(obIDs)) + "\n")
            f.write("train = " + train_file + "\n")
            f.write("test = " + test_file + "\n")
            f.write("names = " + names_file + "\n")
            f.write("backup = backup/\n")
    else:
        with open(paths_folders[2] + "/" + "obj.data", "w") as f:
            f.write("classes = " + str(len(obIDs)) + "\n")
            f.write("train = " + train_file + "\n")
            f.write("test = " + test_file + "\n")
            f.write("names = " + names_file + "\n")
            f.write("backup = backup/\n")

    files = []
    labels = []

    # r=root, d=directories, f = files

    for r, d, f in os.walk(str(paths_folders[1] + "/")):
        for file in f:
            if '.txt' in file:
                labels.append(os.path.join(r, file))

    for imagePath in sorted(paths.list_images(paths_folders[1])):
        files.append(imagePath)

    # set a fixed seed, so results can be replicated by enforcing the same splits every time the script is executed
    # the optional parameter 'random_state' can be used to set a fixed seed. (By default "np.random")
    if k_fold[1] is None:
        files, labels = shuffle(files, labels, random_state=0)
        num_train_examples = int(np.floor(len(files) * (1 - amountTest)))

        print("Using", num_train_examples, "training images and",
              int(np.floor(len(files) - (len(files) * (1 - amountTest)))), "test images. (" + str(amountTest * 100),
              "%)")

        files_train, labels_train = files[0:num_train_examples], labels[0:num_train_examples]
        files_test, labels_test = files[num_train_examples:], labels[num_train_examples:]
    else:
        if len(files) > k_fold[0]:
            files, labels = shuffle(files, labels, random_state=0)
            # use k_fold crossvalidation and return the defined split for Test and Train data
            kf = KFold(n_splits=k_fold[0])
            # return the defined split
            kf_id = 0
            for train_index, test_index in kf.split(labels):
                if kf_id == k_fold[1]:
                    files_train = [files[i] for i in train_index]
                    labels_train = [labels[i] for i in train_index]
                    files_test = [files[i] for i in test_index]
                    labels_test = [labels[i] for i in test_index]
                    break
                else:
                    kf_id += 1

            print("Using", len(train_index), "training images and",
                  len(test_index), "test images. (" , round(100 / k_fold[0]), "%)")
        else:
            # if fewer files are passed then required for the defined split, return empty lists
            files_train, files_test = [],[]

    # create train.txt and test.txt files, containing the locations of the respective image files
    if custom_name is not None:
        with open(paths_folders[2] + "/" + custom_name +"_train.txt", "w") as f:
            for file in range(len(files_train)):
                f.write("data/obj/" + files_train[file].split("\\")[-1] + "\n")

        with open(paths_folders[2] + "/" + custom_name +"_test.txt", "w") as f:
            for file in range(len(files_test)):
                f.write("data/obj/" + files_test[file].split("\\")[-1] + "\n")
    else:
        with open(paths_folders[2] + "/" + "train.txt", "w") as f:
            for file in range(len(files_train)):
                f.write("data/obj/" + files_train[file].split("\\")[-1] + "\n")

        with open(paths_folders[2] + "/" + "test.txt", "w") as f:
            for file in range(len(files_test)):
                f.write("data/obj/" + files_test[file].split("\\")[-1] + "\n")

    print("Successfully created all required files!")


def LoadTrainingData(file):
    """
    :param file: location of train.txt file
    :return: list of all paths to training data files
    """
    trainingDataPaths = []
    with open(file) as f:
        for line in f:
            trainingDataPaths.append(line[:-1])

    return trainingDataPaths


def LoadSyntheticData(file, res=(128, 128), return_all=False, class_value=4, display_img=True, display_markers=False):
    """
    :param file: .csv file containing the name of each image and all limb and bounding box coordinates
    :param res: resolution of input sample
    :param return_all: return all input (if false, only returns relevant data for YOLO)
    :param class_value: assigned class value TODO: read class / weight from file as well
    :param display_img: show bounding boxes and locations of markers
    :return: list containing [img_path, class, centre_x, centre_y, bounding_box_width, bounding_box_height]
    """
    # TODO: CORRECT BOUDNING BOXES! SO FAR THEY APPEAR TO BE RANDOMISED!
    path_split = file.split("\\")[:-1]
    path_to_files = ""
    for folder in path_split:
        path_to_files += folder + "\\"

    all_img_data = []
    with open(file) as f:
        for line in f:
            img_data = []
            img_data_temp = line.split(',')
            for attribute in img_data_temp:
                img_data.append((attribute.split('=')))
                # set the all negative pixel values to 0
                if img_data[-1][0][-1] == "X":
                    img_data[-1][1] = np.maximum(np.round(float(img_data[-1][1]) / res[0], 7), 0)
                elif img_data[-1][0][-1] == "Y":
                    img_data[-1][1] = np.maximum(np.round(float(img_data[-1][1]) / res[1], 7), 0)
                else:
                    img_data[-1] = img_data[-1][0]
            all_img_data.append(img_data)

    if not return_all:
        output = []
        for img in all_img_data:
            img_path = path_to_files + "\\" + img[0] + ".png"

            print(img_path)

            bounding_box_coords = img[19:23]
            print(bounding_box_coords)

            bb_width = bounding_box_coords[2][1] - bounding_box_coords[0][1]
            print("Bounding box width:", bb_width)
            bb_height = bounding_box_coords[3][1] - bounding_box_coords[1][1]
            print("Bounding box height:", bb_height)

            centre_x = bounding_box_coords[0][1] + bb_width / 2
            print("centre_x", centre_x)
            centre_y = bounding_box_coords[1][1] + bb_height / 2
            print("centre_y", centre_x)

            output.append([img_path, class_value, centre_x, centre_y, bb_width, bb_height])

            if display_img:
                img_display = cv2.imread(img_path)

                cv2.rectangle(img_display, (int(img[19][-1] * res[0]), int(img[20][-1] * res[0])),
                              (int(img[21][-1] * res[0]), int(img[22][-1] * res[0])), (0, 255, 0))
                cv2.circle(img_display, (int(centre_x * res[0]), int(centre_y * res[1])), 3, (100, 0, 255), -1)

                if display_markers:
                    # only read every other entry, as X Y are in individual cells
                    for label in range(1, len(img[:]), 2):
                        if img[label][0][0:11] != "BoundingBox":
                            cv2.circle(img_display, (int(img[label][-1] * res[0]), int(img[label + 1][-1] * res[1])), 1,
                                       (0, 0, 10 * label), -1)

                cv2.imshow("annotated_sample", cv2.resize(img_display, dsize=(256, 256)))
                cv2.waitKey(1)
        cv2.destroyAllWindows()
        return output

    else:
        return all_img_data


def writeLoadedSynthetic(data):
    """
    writes syntehtic data files to data/obj/
    :param data: loaded synthetic data
    """
    for img in data:
        file = cv2.imread(img[0])
        cv2.imwrite("obj\\" + img[0].split('\\')[-1][:-4] + "_synth" + ".JPG", gray_file)

        with open("obj\\" + img[0].split('\\')[-1][:-4] + "_synth" + ".txt", "w") as f:
            # [img_path, class, centre_x, centre_y, bounding_box_width, bounding_box_height]

            line = ""
            for element in img[1:]:
                line += str(element) + " "

            f.write(line[:-1])


def addSyntheticToTrainingData(train, synth):
    """
    write out new shuffled training data file
    :param train: locations of all existing training data files
    :param synth: generated data
    """
    # create list of generated data with adjusted paths
    synth_paths = []
    for img in synth:
        synth_paths.append("data/obj/" + img[0].split('\\')[-1][:-4] + " 0.0050" + ".JPG")

    new_train = shuffle(train + synth_paths, random_state=0)

    # create NEW_train.txt file, containing the locations of the respective image files
    with open("data" + "/" + "SYNTH_train.txt", "w") as f:
        for file in new_train:
            f.write(file + "\n")


def createBinaryDatasets(obj_path, single_class=4):
    """
    sets images with the desired class to label 1, all others to 0
    :param obj_path: obj folder containing all JPG and TXT files
    :param single_class: which class is supposed to be set to 1
    """
    files = [],

    # r=root, d=directories, f = files
    for r, d, files in os.walk(obj_path):
        for file in files:
            if '.txt' in file:
                file_path = obj_path + "\\" + file

                with open(file_path, "r") as f:
                    # read class from txt file
                    print(file_path)
                    content = f.read().split(" ")
                    label = int(content[0])
                    if label == single_class:
                        line = "1"
                        print("Correct class")
                    else:
                        line = "0"
                        print("WHAT THE FUCK IS THIS")

                with open(file_path, "w") as f:
                    content[0] = line
                    adjusted_content = ' '.join(content)
                    print(adjusted_content + "\n")

                    f.write(adjusted_content)


if __name__ == "__main__":
    """
    
    1) Current version of video data performs detection only, without weight estimation
    
    """
    """
    path = 'export_2019-06-28_19-24-05/'
    video_name = "2019-06-28_19-24-05"
    cap = cv2.VideoCapture(video_name + ".mp4")

    numFramesMax = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

    tracks = import_tracks(path, numFramesMax, export=False)
    # display_video(cap, tracks, show=(380, 3600), scale=0.4)
    
    video_to_dataset(video=cap, tracks=tracks, ROI=((95, 1023), (180, 1842)), use_frames=(400, 1000),
                     video_name=video_name)
    
    createCustomFiles(obIDs=["Ant"], amountTest=0.1)
    """

    """
    
    2) single image version specifically used to perform weight estimation
    
    """

    # C920_Files = "I:\\Antrax\\Weight_Estim_train_C920"
    # DSLR_Files = "I:\\Antrax\\Weight_Estim_train_DSLR"

    # image_paths = [C920_Files, DSLR_Files]

    # class_names = image_patch_to_dataset(input_paths=image_paths)

    # createCustomFiles(obIDs=class_names, amountTest=0.1)

    """
    
    3) add synthetic data to existent training data set
    
    """
    """
    # specify the location of both files
    training_data_file = "I:\\Antrax\\AntraxV2\\trained_real_data_corrected\\data\\train.txt"
    synthetic_data_file = "I:\\Antrax\\synthetic_data\\0.0050_synth_15_03_20\\AntData.csv"

    # load training data list
    training_data = LoadTrainingData(training_data_file)
    # load synthetic data files
    synthetic_data = LoadSyntheticData(synthetic_data_file, res=(128, 128), display_markers=False)
    # create labeled files and write them to the obj folder
    writeLoadedSynthetic(synthetic_data)

    addSyntheticToTrainingData(training_data, synthetic_data)
    """

    """
    
    4) create binary classification dataset from existing dataset.
    As this is only for testing purposes, edit cfg and obj.names files yourself. 
    
    """
    dataset_path = "I:\\Antrax\\AntraxV2\\synthetic_data_medium_only_BINARY\\data\\obj"

    createBinaryDatasets(obj_path=dataset_path, single_class=4)

    """
    HOW TO TRAIN DARKNET:
    1. place .cfg file in /cfg
    2. place obj folder with all images and labels into /data
    3. place train.txt, test.txt, obj.names, and obj.data into the /data folder
    
    To train run from within darknet-master
    darknet.exe detector train data/obj.data cfg/yolov3_ant_weight.cfg yolov3.weights
    
    
    For MULTI-GPU training run the above command and stop the training after 1000 iterations, then:
    darknet.exe detector train data/obj.data cfg/yolov3_ant_weight.cfg backup/yolov3_ant_weight_501000.weights -gpus 0,1
    darknet.exe detector train data/obj.data cfg/yolov4_ant_weight.cfg .\backup\yolov4_ant_weight_8000.weights -gpus 1
    
    the last argument is calling which GPUs to use explicitly.
    """
