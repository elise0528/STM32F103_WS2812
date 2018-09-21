import os
import json

from xml.dom.minidom import parse


def search_file(file_dir, sname):
    for root, dirs, files in os.walk(file_dir):
        for file in files:
            if os.path.splitext(file)[1] == sname:
                print('find {}.uvprojx......'.format(os.path.splitext(file)[0]))
                return os.path.join(root, file)


def get_config_tuple(file_name_path):
    print('Getting configuration information......')

    list_define = ['__CC_ARM']
    list_include_path = []

    if os.getenv('KEIL_ROOT'):
        list_include_path.append((os.getenv('KEIL_ROOT') + '/ARM/ARMCC/include').replace('\\', '/'))

    list_include_path.append('${workspaceRoot}/**')
    # print(list_include_path)

    # parse()Ëé∑ÂèñDOMÂØπË±°
    dom = parse(file_name_path)
    # Ëé∑ÂèñÊ†πËäÇÁÇ?
    root = dom.documentElement

    tag_cads = root.getElementsByTagName('Cads')[0]

    tag_define = tag_cads.getElementsByTagName('Define')[0]
    tag_include_path = tag_cads.getElementsByTagName('IncludePath')[0]

    list_define += tag_define.childNodes[0].nodeValue.split(',')
    list_include_path += tag_include_path.childNodes[0].nodeValue.replace(' ', '') \
        .replace('..', '${workspaceRoot}').split(';')

    # print(list_define)
    # print(list_include_path)

    return list_define, list_include_path


def set_vscode_config(config, path):
    print('Configuration files are being generated......')

    dict_config = {
        "configurations": [
            {
                "name": "Win32",
                "includePath": [
                ],
                "defines": [
                ]
            }
        ]
    }
    dict_config['configurations'][0]['defines'] = config[0]
    dict_config['configurations'][0]['includePath'] = config[1]

    # print(dict_config['configurations'])

    if os.path.isdir(path + '\.vscode') is False:
        os.mkdir(path + '\.vscode')
    with open(path + '\.vscode\c_cpp_properties.json', 'w') as f:
        json.dump(dict_config, f, indent=4, sort_keys=True)


if __name__ == '__main__':
    current_path = os.getcwd()
    # print(current_path)
    # current_path = 'D:\WindowsApps\AppData\CubeMXProjects\LedDome'
    file_path = search_file(current_path, ".uvprojx")
    # print(file_path)
    tuple_config = get_config_tuple(file_path)
    # print(tuple_config)
    set_vscode_config(tuple_config, current_path)
    input('«Î∞¥»Œ“‚º¸ÕÀ≥ˆ......')
