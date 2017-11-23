# coding: utf-8
import json
import pathlib

import click
from ebooklib import epub


@click.group()
def cli():
    pass


@cli.command("make")
@click.option("--contents-file", required=True, help="contents file path")
@click.option("--output-file", required=True, help="output file path")
@click.option("--title", help="book title")
def make(contents_file, output_file, title):
    f = open(contents_file, encoding='utf-8').read()
    contents_doc = json.loads(f)

    book = epub.EpubBook()

    book.set_title(title)

    contents = contents_doc['contents']
    chapter_list = []
    toc_list = []

    for a_chapter in contents:
        name = a_chapter['name']
        file_path = a_chapter['file_path']
        file_name = pathlib.PurePath(file_path).name

        chapter = epub.EpubHtml(title=name, file_name=file_name)
        with open(file_path, encoding='utf-8') as f:
            chapter_html = f.read()
            chapter.content = chapter_html

        book.add_item(chapter)
        chapter_list.append(chapter)
        toc_list.append(epub.Link(file_name, name, file_name))

    book.toc = tuple(toc_list)
    book.add_item(epub.EpubNcx())
    book.add_item(epub.EpubNav())

    book.spine = ['nav'] + chapter_list

    epub.write_epub(output_file, book, {})


if __name__ == "__main__":
    cli()
