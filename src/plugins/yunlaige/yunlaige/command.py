# coding: utf-8
import click
import json

import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin


def get_novel_contents_info(url, proxy_config, socket_config) -> dict:
    response = requests.get(url, proxies=proxy_config)
    bs_object = BeautifulSoup(response.content, "html.parser", from_encoding='gb18030')

    content_container_node = bs_object.select_one('#contenttable')
    link_nodes = content_container_node.findAll('a')

    title_div = bs_object.select_one('.title')
    title_string = title_div.find('h1').string[:-4]
    author_string = str(title_div.find('span').string)[3:]

    contents = []
    for a_link in link_nodes:
        if 'href' not in a_link.attrs:
            continue
        href = a_link['href']
        link = urljoin(url, href)
        name = str(a_link.string)
        contents.append({
            'name': name,
            'link': link
        })
    return {
        'title': title_string,
        'author': author_string,
        'contents': contents,
        'content_url': url
    }


def get_novel_chapter(url, proxy_config, socket_config):
    response = requests.get(url, proxies=proxy_config)
    bs_object = BeautifulSoup(response.content, "html.parser", from_encoding='gb18030')

    title_node = bs_object.select_one('.bookname h1')

    content_node = bs_object.select_one('#content')

    paragraphs = content_node.strings

    html_chapter = BeautifulSoup("<div></div>", "html5lib")
    main_div = html_chapter.div
    html_title_node = html_chapter.new_tag('h1')
    html_title_node.string = title_node.string
    main_div.append(html_title_node)

    html_content_node = html_chapter.new_tag('div')
    main_div.append(html_content_node)

    for a_paragraph in paragraphs:
        p_text = a_paragraph.string.strip()
        p_node = html_chapter.new_tag('p')
        p_node.string = p_text
        html_content_node.append(p_node)

    return html_chapter.prettify()


@click.group()
def cli():
    pass


@cli.command('contents')
@click.option('--url', required=True, help="url for novel's contents")
@click.option('--proxy-config', default='{}', help="proxy config using in requests")
@click.option('--socket-config', default='{}', help="socket config")
def contents(url, proxy_config, socket_config):
    """
get contents from url

socket config:
  {
    server:
    {
       host: server_host,
       port: server_port
    }
  }
    """
    proxy_config_dict = json.loads(proxy_config)
    socket_config_dict = json.loads(socket_config)
    novel_contents_info = get_novel_contents_info(url, proxy_config_dict, socket_config=socket_config_dict)
    result = {
        'app': 'novel_downloader_command_tool',
        'type': 'command_result',
        'data': {
            'plugin': 'biquge',
            'request': {
                'command': 'contents',
                'url': url
            },
            'response': novel_contents_info
        }
    }

    # print utf-8 strings to std out.
    import sys
    sys.stdout = open(1, 'w', encoding='utf-8', closefd=False)
    output = json.dumps(result, indent=2, ensure_ascii=False)
    print(output)


@cli.command('chapter')
@click.option('--url', required=True, help="url for novel's contents")
@click.option('--proxy-config', default='{}', help="proxy config using in requests")
@click.option('--socket-config', default='{}', help="socket config")
def chapter(url, proxy_config, socket_config):
    """
get chapter from url

socket config:
  {
    server:
    {
       host: server_host,
       port: server_port
    }
  }
    """
    proxy_config_dict = json.loads(proxy_config)
    socket_config_dict = json.loads(socket_config)
    chapter_html = get_novel_chapter(
        url=url,
        proxy_config=proxy_config_dict,
        socket_config=socket_config_dict
    )
    result = {
        'app': 'novel_downloader_command_tool',
        'type': 'command_result',
        'data': {
            'plugin': 'biquge',
            'request': {
                'command': 'chapter',
                'url': url
            },
            'response': {
                'chapter': chapter_html
            }
        }
    }

    # print utf-8 strings to std out.
    import sys
    sys.stdout = open(1, 'w', encoding='utf-8', closefd=False)
    print(json.dumps(result, indent=2, ensure_ascii=False))


if __name__ == "__main__":
    cli()
