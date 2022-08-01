#!/usr/bin/env python

import xml.etree.ElementTree as etree
import yaml
from uuid import uuid4
from typing import NamedTuple
from xml.dom import minidom
from os import path
from argparse import ArgumentParser

args = ArgumentParser(description='QtCreator project file generator')
args.add_argument(
  '--root',
  default=path.dirname(path.realpath(path.dirname(__file__))),
  help='Project root')

class BuildName(NamedTuple):
  name: str
  architecture: str
  variant: str

def create_project(args):
  def get_build_config(root: str):
    with open(f'{root}/.build.yml') as build_yml:
      return yaml.load(build_yml, Loader=yaml.loader.SafeLoader)
    return None
  
  def parse_target_name(name: str):
    components = name.split(':')
    return BuildName(
      name=components[0][0].capitalize() + components[0][1:],
      architecture=components[1],
      variant=components[2]
    )
  def normalize_build_name(name: str):
    components = name.split(':')
    return f'{components[0]}-{components[1]}_{components[2]}'

  def field(name, value, **attributes):
    return {name: value, '_attributes': attributes}

  def valuestring(value, **attributes):
    return field('value', value, type='QString', **attributes)

  def valueint(value, **attributes):
    return field('value', value, type='int', **attributes)

  def valuebool(value, **attributes):
    return field('value', value, type='bool', **attributes)

  def valuebytes(value, **attributes):
    return field('value', value, type='QByteArray', **attributes)

  def valuemap(value, **attributes):
    return field('valuemap', value, type='QVariantMap', **attributes)

  def valuelist(value, **attributes):
    return field('valuemap', value, type='QVariantList', **attributes)

  def create_cmake_config(root, target_name, config):
    build_config = get_build_config(root)
    compiler_version = build_config['toolchain']['version']
    toolchain_root = f'{root}/multi_build/{normalize_build_name(target_name)}/compiler-{compiler_version}'

    return valuelist([
      valuestring(f'CMAKE_BUILD_TYPE:STRING={config}'),
      valuestring(f'TOOLCHAIN_ROOT:STRING={toolchain_root}'),
      valuestring(f'TOOLCHAIN_PREFIX:STRING={normalize_build_name(target_name)}'),
    ], key='CMake.Configuration')

  def create_build_steps(name, target, i=0, args=[]):
    return valuemap(key=f'ProjectExplorer.BuildConfiguration.BuildStepList.{i}', value=[
      valuemap(key='ProjectExplorer.BuildConfiguration.BuildStepList.Step.0', value=[
        valuestring(key='CMakeProjectManager.MakeStep.AdditionalArguments', value=args),
        valuelist(key='CMakeProjectManager.MakeStep.BuildTargets', value=[
          valuestring(target),
        ]),
        valuestring(key='ProjectExplorer.BuildStep.Enabled', value='true'),
        valuestring(key='ProjectExplorer.ProjectConfiguration.Id', value='CMakeProjectManager.MakeStep')
      ]),
      valueint(key='ProjectExplorer.BuildStepList.StepsCount', value=1),
      valuestring(key='ProjectExplorer.ProjectConfiguration.DefaultDisplayName', value=name),
      valuestring(key='ProjectExplorer.ProjectConfiguration.DisplayName', value=name),
      valuestring(key='ProjectExplorer.ProjectConfiguration.Id', value=f'ProjectExplorer.BuildSteps.{name}'),
    ])

  def create_build_config(root, target_name, config, i=0):
    build_config = get_build_config(root)
    build_dir = f'{root}/multi_build/{normalize_build_name(target_name)}'
    compiler_version = build_config['toolchain']['version']

    return valuemap([
      valuestring(key='ProjectExplorer.ProjectConfiguration.DisplayName', value=config),
      valuestring(key='ProjectExplorer.ProjectConfiguration.Id', value='CMakeProjectManager.CMakeBuildConfiguration'),
      valuestring(key='ProjectExplorer.BuildConfiguration.BuildDirectory', value=build_dir),
      
      create_cmake_config(root, config=config, target_name=target_name),
      
      create_build_steps('Build', 'all', i=0),
      create_build_steps('Clean', 'clean', i=1),
      valueint(key='ProjectExplorer.BuildConfiguration.BuildStepListCount', value=2),

      valuebool(key='ProjectExplorer.BuildConfiguration.ClearSystemEnvironment', value='false'),
      valuelist(key='ProjectExplorer.BuildConfiguration.UserEnvironmentChanges', value=[
        valuestring(f'TOOLCHAIN_ROOT={build_dir}/compiler-{compiler_version}'),
        valuestring(f'TOOLCHAIN_PREFIX={normalize_build_name(target_name)}'),
      ]),
    ], key=f'ProjectExplorer.BuildConfiguration.{i}')

  def create_run_config(root, i=0):
    return valuemap([

    ], key=f'ProjectExplorer.Target.RunConfiguration.{i}')

  def create_deploy_config(root):
    return valuemap([

    ], key='ProjectExplorer.Target.DeployConfiguration.0')

  def create_target(root, target_name, i):
    target_parts = parse_target_name(target_name)
    pretty_name = f'{target_parts[0]} {target_parts[2]} ({target_parts[1]})'

    return {
      'data': [
        {'variable': f'ProjectExplorer.Project.Target.{i}'},
        valuemap([
          valuestring(key='ProjectExplorer.ProjectConfiguration.DefaultDisplayName', value=pretty_name),
          valuestring(key='ProjectExplorer.ProjectConfiguration.DisplayName', value=pretty_name),
          valuestring(key='ProjectExplorer.ProjectConfiguration.Id', value=f'{{{uuid4().hex}}}'),
          
          create_build_config(root, config='Debug', target_name=target_name, i=0),
          create_build_config(root, config='Release', target_name=target_name, i=1),
          valueint(key='ProjectExplorer.Target.BuildConfigurationCount', value=2),

          create_deploy_config(root),
          valueint(key='ProjectExplorer.Target.DeployConfigurationCount', value=1),
          
          create_run_config(root),
          valueint(key='ProjectExplorer.Target.RunConfigurationCount', value=1),
        ])
      ]
    }
  
  return [
    {'qtcreator': [
      create_target(args.root, target_name='desktop:x86_64-buildroot-linux-gnu:multi', i=0),
      {'data': [{'variable': 'ProjectExplorer.Project.TargetCount'}, valueint(1)]},
      {'data': [{'variable': 'Version'}, valueint(22)]},
    ]}
  ]

def generate_document(source):
  def get_element_name(o):
    for key in o:
      if key != '_attributes':
        return key
    raise AttributeError('no key found')

  def get_element_attributes(o):
    for key in o:
      if key == '_attributes':
        return o[key]
    return {}

  def generate_tree(parent: etree.Element, children: list):
    for child in children:
      name = get_element_name(child)
      attrs = get_element_attributes(child)
      e = etree.SubElement(parent, name, attrs)

      nested = child[name]
      if type(nested) == list:
        generate_tree(e, child[name])
      else:
        e.text = str(nested)

  root_element = etree.Element('qtcreator')
  generate_tree(root_element, source)
  return root_element

project_file = create_project(args.parse_args())

for root in project_file[0]:
  document = generate_document(project_file[0][root])
  preamble = '<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE QtCreatorProject>\n'.encode('utf8')
  encoded = preamble + etree.tostring(document, xml_declaration=False)
  print(minidom.parseString(encoded).toprettyxml(indent='  '))

