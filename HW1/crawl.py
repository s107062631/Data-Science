from bs4 import BeautifulSoup
from urllib.request import urlopen
import sys
import re

argument = sys.argv
hw_url = 'https://arxiv.org/search/?searchtype=all&query='+argument[1]+'&abstracts=show&size=100&order='
hw_html = urlopen(hw_url).read()
hw_soup = BeautifulSoup(hw_html, 'html.parser')
target = hw_soup.find_all('p', "list-title is-inline-block")

allpaper = list()
stop_words = list()
words_dict = {}

with open('paper_info.txt', 'w') as f:
    for i in target:
        f.write(i.a['href']+'\n')
        paper_html = urlopen(str(i.a['href'])).read()
        papersoup = BeautifulSoup(paper_html, 'html.parser')
        papertarget = papersoup.find('h1', 'title mathjax')
        attr = papertarget.text.strip().replace('Title:', "")
        f.write(attr+'\n')
        papertarget = papersoup.find('div', 'authors')
        attr = papertarget.text.strip().replace('Authors:', "").replace("'", "").split(",")
        for authors in attr:
            f.write(authors+';')
        f.write('\n')
print("Paper name, URL, Authors, Done")

f_stop = open('stop_words.txt','r')
for i in f_stop.readlines():
    stop_words.append(i.replace('\n',''))

for i in target:
    paper_html = urlopen(str(i.a['href'])).read()
    papersoup = BeautifulSoup(paper_html, 'html.parser')
    papertarget = papersoup.find('blockquote', 'abstract mathjax')
    attr = papertarget.text.strip().replace('Abstract:', "")
    new_str = re.sub('[;~!@#$%^&*><.?,]', ' ', attr).lower().split()
    for j in new_str:
        if j not in stop_words:
            if j not in words_dict:
                words_dict[j] = 1
            else:
                words_dict[j] += 1

sorted_word = sorted(words_dict.items(), key=lambda d: d[1], reverse=True)
with open('frequent_word.txt', 'w') as f:
    for j in sorted_word:
        f.write(j[0] + ' ' + str(j[1]) + '\n')
print('Frequent words Done')