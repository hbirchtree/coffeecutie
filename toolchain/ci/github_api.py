#!/usr/bin/env python3
from collections import namedtuple
from argparse import ArgumentParser
import requests
import json

from sys import stderr


REQUEST_POST = 0
REQUEST_GET  = 1

GH_TOKEN = None
GH_API = 'https://api.github.com'

_verbose = False
args = None


class GithubRequestData:
    def __init__(self):
        self.type = ''
        self.headers = {}
        self.content = {}
        self.status_code = 0


def printerr(s):
    #print(s, file=stderr)
    pass


def rest_request(url, headers, data, form=None, rtype=REQUEST_GET):
    if _verbose:
        printerr('Request: %s %s %s %s' % (rtype, url, headers, type(data)))
        printerr(data)
    #print(url, headers, data)
    if rtype == REQUEST_GET:
        return requests.get(url, headers=headers)
    elif rtype == REQUEST_POST:
        return requests.post(url, data=data, params=form, headers=headers)
    else:
        raise RuntimeError('Invalid request type')


def gh_request(endpoint, data=None, rtype=REQUEST_GET, headers=None, form=None):
    if headers is None:
        headers = {}

    if 'Accept' not in headers:
        headers['Accept'] = 'application/vnd.github.v3+json'

    if GH_TOKEN is not None and GH_TOKEN != "":
        headers['Authorization'] = 'token %s' % GH_TOKEN

    url = None
    if endpoint.startswith('https'):
        url = endpoint
    else:
        url = '%s%s' % (GH_API, endpoint)

    resp = rest_request(url,
                        headers=headers,
                        data=data,
                        rtype=rtype,
                        form=form)

    resp_ = GithubRequestData()
    resp_.type = resp.headers['Content-Type']
    resp_.headers = resp.headers
    resp_.content = resp.content
    resp_.status_code = resp.status_code

    jsondata = False
    if resp_.type.startswith('application/json'):
        resp_.content = json.loads(resp_.content.decode())
        jsondata = True

    if 'Link' in resp.headers and 'next' in resp.links and jsondata:
        #printerr('Using link header')
        r = gh_request(resp.links['next']['url'], headers=headers, data=None, rtype=rtype)
        if type(r.content) == list and type(resp_.content) == list:
            resp_.content += r.content
        else:
            raise RuntimeError('Got Link header, but not way to merge it')

    return resp_


def gh_errorhandle(resp):
    if resp.status_code >= 400:
        printerr('Got response: %s' % resp.status_code)
        printerr('Content: \n%s' % resp.content)
        exit(1)


def gh_get_release(repo, release):
    release_resp = gh_request('/repos/%s/releases' % repo)
    gh_errorhandle(release_resp)
    release_data = release_resp.content
    for r in release_data:
        if r['tag_name'] == release:
            release_data = r
            break
    if type(release_data) == list:
        return {}
    return release_data


def handle_cmd(action, cat, item, select):
    if action == 'list':
        import re
        if cat == 'release':
            # Just get and format the list of releases
            resp = gh_request('/repos/%s/releases' % item)
            gh_errorhandle(resp)
            j = resp.content
            patterns = []
            for e in select:
                patterns += [re.compile(e)]
            for r in j:
                match = False
                if len(patterns) < 1:
                    match = True
                for e in patterns:
                    match = match or e.match(r['tag_name'])
                if not match:
                    continue
                print('%s|%s|%s' % (
                    r['id'],
                    item,
                    r['tag_name']
                ))
        elif cat == 'asset':
            item, release = item.split(':')
            # First, get list of releases to get the release ID
            release_data = gh_get_release(item, release)
            assert ('id' in release_data)
            release_id = release_data['id']
            if release_id is None:
                printerr('Release not found')
                exit(1)
            resp = gh_request('/repos/%s/releases/%s/assets' % (item, release_id))
            gh_errorhandle(resp)
            j = resp.content
            patterns = []
            for e in select:
                patterns += [re.compile(e)]
            for a in j:
                match = False
                if len(patterns) < 1:
                    match = True
                for e in patterns:
                    match = match or e.match(a['name'])
                print('%s|%s|%s|%s|%s|%s|%s' % (
                    item, release,
                    a['id'],
                    release_data['name'],
                    a['name'],
                    '',
                    release_data['tag_name']
                ))
        elif cat == 'tag':
            resp = gh_request('/repos/%s/tags' % (item,))
            gh_errorhandle(resp)
            patterns = []
            for e in select:
                patterns += [re.compile(e)]
            for r in resp.content:
                match = False
                if len(patterns) < 1:
                    match = True
                for e in patterns:
                    match = match or e.match(r['commit']['sha'])
                if not match:
                    continue
                print('%s|%s|%s' % (
                    item,
                    r['name'],
                    r['commit']['sha']
                ))
    elif action == 'pull':
        if cat == 'asset':
            resp = gh_request('/repos/%s/releases/assets/%s' % (item, select[0]))
            gh_errorhandle(resp)
            jdata = resp.content
            assert ('browser_download_url' in jdata and 'name' in jdata)
            printerr('Downloading %s bytes to %s' % (jdata['size'], jdata['name']))
            data = gh_request('/repos/%s/releases/assets/%s' % (item, select[0]),
                              headers={'Accept': 'application/octet-stream'})
            gh_errorhandle(data)
            assert (data.type == 'application/octet-stream')
            with open(jdata['name'], mode='wb') as f:
                f.write(data.content)
    elif action == 'push':
        if GH_TOKEN is None:
            raise RuntimeError('No API token, cannot push')

        if cat == 'asset':
            item, release = item.split(':')
            release_data = gh_get_release(item, release)
            if len(release_data) == 0:
                raise RuntimeError('No releases found')
            assert ('id' in release_data)
            with open(select[0], mode='rb') as f:
                data = f.read()
                endpoint = release_data['upload_url']
                endpoint = endpoint.replace('{?name,label}', '')
                upload = rest_request('%s' % endpoint,
                                      data=data,
                                      rtype=REQUEST_POST,
                                      headers={
                                          'Content-Type': 'application/octet-stream',
                                          'Content-Length': '%s' % len(data),
                                          'Authorization': 'token %s' % GH_TOKEN
                                      },
                                      form={
                                          'name': select[0]
                                      })
                gh_errorhandle(upload)
        elif cat == 'status':
            item, sha = item.split(':')
            if len(select) != 2:
                raise RuntimeError("Wrong amount of arguments, received: %s" % select)

            assert(select[0] in ['success', 'pending', 'error', 'failure'])
            assert(len(select[1]) < 1024)

            status_data = {
                'state': select[0],
                'description': select[1],
                'context': args.context
            }

            if args.url is not None:
                status_data['target_url'] = args.url

            status = gh_request('/repos/%s/statuses/%s' % (item, sha),
                                data=json.dumps(status_data),
                                rtype=REQUEST_POST,
                                headers={
                                    'Content-Type': 'application/json'
                                })
            gh_errorhandle(status)
        elif cat == 'release':
            if len(select) != 3:
                raise RuntimeError("Wrong amount of arguments, received: %s" % select)

            release_data = {
                'tag_name': select[0],
                'name': select[1],
                'body': select[2],
                'prerelease': True
            }

            status = gh_request('/repos/%s/releases' % (item,),
                                data=json.dumps(release_data),
                                rtype=REQUEST_POST,
                                headers={
                                    'Content-Type': 'application/json'
                                })
            gh_errorhandle(status)


def main():
    global args

    args = ArgumentParser('Github API tool')

    args.add_argument('action', type=str,
                      choices=['list', 'pull', 'push'],
                      help='action to be performed on category')

    args.add_argument('category', type=str,
                      choices=['asset', 'release', 'status', 'tag'],
                      help='category of item to pull')

    args.add_argument('item', type=str,
                      help='item to be inspected/modified')

    args.add_argument('selection', type=str, nargs='*',
                      help='selection or target name')

    args.add_argument('--api-token', dest='token', type=str,
                      help='Github API token',
                      default='')
    args.add_argument('--gh-context', dest='context', type=str,
                      help='Github API context, when identifying this system',
                      default='Coffee GitHub Client')
    args.add_argument('--gh-url', dest='url', type=str,
                      help='Github API context url, if applicable',
                      default=None)

    args.add_argument('--verbose', dest='verbose', action='store_true',
                      help='verbose mode')

    args = args.parse_args()

    global _verbose
    _verbose = args.verbose

    global GH_TOKEN
    GH_TOKEN = args.token

    handle_cmd(args.action, args.category, args.item, args.selection)

if __name__ == '__main__':
    main()
