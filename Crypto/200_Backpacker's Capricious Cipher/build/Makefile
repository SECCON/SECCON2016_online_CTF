# ファイルの作成
TITLE=capricious_cipher

files/${TITLE}.zip:
	rm -rf ${TITLE} files/${TITLE}.zip
	mkdir -p ${TITLE}
	cp src/cipher.rb ${TITLE}
	cp src/flag.txt ${TITLE}
	cd ${TITLE} && ruby cipher.rb
	mkdir -p files
	zip files/${TITLE}.zip ${TITLE}/cipher.rb ${TITLE}/enc.txt ${TITLE}/pub_key.txt
	rm -r ${TITLE}
clean:
