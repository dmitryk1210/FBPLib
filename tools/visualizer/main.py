# This is FBPLib visualization tool

import argparse
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.colors import Normalize

DC_START_CAPTURE_TIME      = "START_CAPTURE_TIME"
DC_STOP_CAPTURE_TIME       = "STOP_CAPTURE_TIME"
DC_INSTANCES_COUNT         = "INSTANCES_COUNT"
DC_INSTANCES_SECTION_BEGIN = "INSTANCES_SECTION_BEGIN"
DC_INSTANCES_SECTION_END   = "INSTANCES_SECTION_END"
DC_INSTANCE_BEGIN          = "INSTANCE_BEGIN"
DC_INSTANCE_END            = "INSTANCE_END"

DC_SEPARATOR = " "

TIME_SCALING_CONST = 1E-6


class Chunk:
    def __init__(self):
        self.threadId  = -1
        self.taskName  = ""
        self.startTime = -1
        self.endTime   = -1

    def initFromList(self, list):
        self.threadId  = int(list[0])
        self.taskName  = list[1]
        self.startTime = int(list[2])
        self.endTime   = int(list[3])


class Instance:
    def __init__(self):
        self.chunks = []


class Thread:
    def __init__(self):
        self.chunks = []

    def getChunkAtTimePoint(self, timePoint):
        for chunk in self.chunks:
            if chunk.startTime < timePoint < chunk.endTime:
                return chunk
        return None


class Capture:
    def __init__(self):
        self.startTime      = -1
        self.endTime        = -1
        self.instancesCount = -1
        self.instances      = []
        self.threads        = {}
        self.taskNameToIdx  = {}
        self.taskNames      = []

    def isAllParametersValid(self):
        return (self.startTime != -1) and (self.endTime != -1) and (self.instancesCount != -1)

    def processAdditionalData(self):
        self.prepareTasksList()
        self.prepareThreadsData()

    def prepareTasksList(self):
        for instance in self.instances:
            for chunk in instance.chunks:
                if chunk.taskName not in self.taskNameToIdx:
                    self.taskNameToIdx[chunk.taskName] = len(self.taskNameToIdx)
                    self.taskNames.append(chunk.taskName)

    def prepareThreadsData(self):
        for instance in self.instances:
            for chunk in instance.chunks:
                if chunk.threadId not in self.threads:
                    self.threads[chunk.threadId] = Thread()
                self.threads[chunk.threadId].chunks.append(chunk)


def trySetParameter(capture, line):
    splitted = line.split(DC_SEPARATOR)
    if len(splitted) < 2:
        return False
    if splitted[0] == DC_START_CAPTURE_TIME:
        capture.startTime = int(splitted[1])
    elif splitted[0] == DC_STOP_CAPTURE_TIME:
        capture.endTime = int(splitted[1])
    elif splitted[0] == DC_INSTANCES_COUNT:
        capture.instancesCount = int(splitted[1])
    else:
        return False
    return True


def tryReadChunk(instance, line):
    splitted = line.split(DC_SEPARATOR)
    if len(splitted) < 4:
        return False
    chunk = Chunk()
    chunk.initFromList(splitted)
    instance.chunks.append(chunk)
    return True


def parseInputFile(name):
    capture = Capture()
    with open(name, 'r') as f:
        line = f.readline().strip('\n\r')
        while line != DC_INSTANCES_SECTION_BEGIN:
            if line == "":
                return None
            trySetParameter(capture, line)
            line = f.readline().strip('\n\r')

        if not capture.isAllParametersValid():
            return None

        for i in range(capture.instancesCount):
            line = f.readline().strip('\n\r')
            if line != DC_INSTANCE_BEGIN:
                return None
            line = f.readline().strip('\n\r')

            capture.instances.append(Instance())
            while line != DC_INSTANCE_END:
                if line == "":
                    return None
                tryReadChunk(capture.instances[i], line)
                line = f.readline().strip('\n\r')
    capture.processAdditionalData()
    return capture


capture = None


def showThreadsPlot():
    global capture
    if capture is None:
        return

    y = np.arange(len(capture.threads))
    x_values = np.linspace(0, float(capture.endTime - capture.startTime) * TIME_SCALING_CONST, 1024)
    width = 10
    height = 0.5

    fig, ax = plt.subplots(figsize=(12, 10))

    norm = Normalize(0, 1)
    cmap = plt.get_cmap('viridis')

    tasksCount = len(capture.taskNameToIdx)

    colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k']
    taskNameToColor = {}
    for taskName, idx in capture.taskNameToIdx.items():
        taskNameToColor[taskName] = colors[idx]

    for yi, threadObj in zip(y, capture.threads.values()):
        for x in x_values:
            color_value = -1
            currentChunk = threadObj.getChunkAtTimePoint(x / TIME_SCALING_CONST + capture.startTime)
            if currentChunk is not None:
                color_value = (capture.taskNameToIdx[currentChunk.taskName]) / (tasksCount - 1)
            color = 'w' if (color_value == -1) else cmap(norm(color_value))
            #color = 'w' if (color_value == -1) else taskNameToColor[currentChunk.taskName]
            ax.add_patch(patches.Rectangle((x, yi), width / len(x_values), height, color=color))

    ax.set_xlim([0, x_values[-1]])
    ax.set_ylim([0, max(y) + 1])
    ax.set_xlabel('time, ms')
    ax.set_ylabel('processors')
    ax.set_title('Tasks by processors')

    colors = [cmap(i / (tasksCount - 1)) for i in range(tasksCount)]
    labels = list(capture.taskNameToIdx.keys())
    patches_list = [patches.Patch(color=colors[i], label=labels[i]) for i in range(tasksCount)]
    #legend = ax.legend(handles=patches_list, title="Legend", loc='upper right', bbox_to_anchor=(1.15, 1))
    plt.show()


def showTasksPlot():
    global capture
    if capture is None:
        return

    x_values = np.linspace(0, float(capture.endTime - capture.startTime) * TIME_SCALING_CONST, 1024)
    y = np.zeros((len(capture.taskNameToIdx), 1024))

    fig, ax = plt.subplots(figsize=(18, 10))

    norm = Normalize(0, 1)
    cmap = plt.get_cmap('viridis')

    tasksCount = len(capture.taskNameToIdx)

    colors  = [cmap(i / (tasksCount - 1)) for i in range(tasksCount)]

    for i, xi in enumerate(x_values):
        for threadObj in capture.threads.values():
            currentChunk = threadObj.getChunkAtTimePoint(xi / TIME_SCALING_CONST + capture.startTime)
            if currentChunk is not None:
                y[capture.taskNameToIdx[currentChunk.taskName]][i] += 1

    plt.grid()
    ax.set_xlim([0, x_values[-1]])
    ax.set_ylim([0, len(capture.threads) + 1])
    ax.set_xlabel('time, ms')
    ax.set_ylabel('processors')
    ax.set_title('Processors count by task')

    for i, yi in enumerate(y):
        plt.plot(x_values, yi, color=colors[i])

    labels = list(capture.taskNameToIdx.keys())
    patches_list = [patches.Patch(color=colors[i], label=labels[i]) for i in range(tasksCount)]
    legend = ax.legend(handles=patches_list, title="Legend", loc='upper right', bbox_to_anchor=(1.12, 1))
    plt.show()

    for i, yi in enumerate(y):
        fig, ax = plt.subplots(figsize=(18, 10))
        plt.grid()
        ax.set_xlim([0, x_values[-1]])
        ax.set_ylim([0, len(capture.threads) + 1])
        ax.set_xlabel('time, ms')
        ax.set_ylabel('processors')
        ax.set_title(f'Processors count for task {capture.taskNames[i]}')
        plt.plot(x_values, yi)
        plt.show()


def main():
    global capture
    argParser = argparse.ArgumentParser()
    argParser.add_argument('--inpfile', dest='inpfile', type=str, help='Input file name')
    args = argParser.parse_args()

    print(args.inpfile)

    if args.inpfile is not None:
        capture = parseInputFile(args.inpfile)

    if capture is None:
        return

    showTasksPlot()
    showThreadsPlot()
    print(args.inpfile)


if __name__ == "__main__":
    main()

