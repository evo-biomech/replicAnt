import unreal

def openfolder(folderpath):
    import subprocess
    subprocess.call("explorer {}".format(folderpath) , shell=True)
    unreal.log('"{}"'.format(folderpath))
