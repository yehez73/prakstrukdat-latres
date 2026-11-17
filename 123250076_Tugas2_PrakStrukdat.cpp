#include <iostream>
#include <string>

using namespace std;

struct NodeQueue {
    string pelanggan;
    NodeQueue* next;
};

struct NodeStack {
    string jenisAksi;
    string namaMenu;
    string pelanggan;
    NodeStack* next;
};

struct Menu {
    string nama, kategori;
    int harga;
    bool tersedia;
    
    NodeQueue *frontAntrian, *rearAntrian;
    
    Menu *left, *right;
};

NodeStack* topStack = nullptr;

Menu* buatMenu(string nama, string kategori, int harga) {
    Menu* baru = new Menu;
    baru->nama = nama;
    baru->kategori = kategori;
    baru->harga = harga;
    baru->tersedia = true;
    baru->frontAntrian = nullptr;
    baru->rearAntrian = nullptr;
    baru->left = nullptr;
    baru->right = nullptr;
    return baru;
}

void enqueue(Menu* menu, string namaPelanggan) {
    NodeQueue* baru = new NodeQueue;
    baru->pelanggan = namaPelanggan;
    baru->next = nullptr;
    
    if (menu->frontAntrian == nullptr) {
        menu->frontAntrian = menu->rearAntrian = baru;
    } else {
        menu->rearAntrian->next = baru;
        menu->rearAntrian = baru;
    }
}

string dequeue(Menu* menu) {
    if (menu->frontAntrian == nullptr) return "";
    NodeQueue* hapus = menu->frontAntrian;
    string nama = hapus->pelanggan;
    menu->frontAntrian = menu->frontAntrian->next;
    if (menu->frontAntrian == nullptr) {
        menu->rearAntrian = nullptr;
    }
    delete hapus;
    return nama;
}

void pushFrontAntrian(Menu* menu, string namaPelanggan) {
    NodeQueue* baru = new NodeQueue;
    baru->pelanggan = namaPelanggan;
    baru->next = menu->frontAntrian;
    menu->frontAntrian = baru;
    if (menu->rearAntrian == nullptr) {
        menu->rearAntrian = baru;
    }
}

void pushStack(string aksi, string menu, string pelanggan = "") {
    NodeStack* baru = new NodeStack;
    baru->jenisAksi = aksi;
    baru->namaMenu = menu;
    baru->pelanggan = pelanggan;
    baru->next = topStack;
    topStack = baru;
}

NodeStack* popStack() {
    if (topStack == nullptr) return nullptr;
    NodeStack* hapus = topStack;
    topStack = topStack->next;
    return hapus;
}

Menu* insertMenu(Menu* root, string nama, string kategori, int harga) {
    if (root == nullptr) return buatMenu(nama, kategori, harga);
    if (nama < root->nama)
        root->left = insertMenu(root->left, nama, kategori, harga);
    else if (nama > root->nama)
        root->right = insertMenu(root->right, nama, kategori, harga);
    return root;
}

Menu* cariMenu(Menu* root, string nama) {
    if (root == nullptr || root->nama == nama) return root;
    if (nama < root->nama) return cariMenu(root->left, nama);
    return cariMenu(root->right, nama);
}

void tampilMenu(Menu* root) {
    if (root == nullptr) return;
    tampilMenu(root->left);
    cout << "- " << root->nama << " (" << root->kategori << ") | Rp" << root->harga 
         << " | Status: " << (root->tersedia ? "Tersedia" : "Habis") << endl;
    tampilMenu(root->right);
}

Menu* cariMin(Menu* root) {
    while (root && root->left != nullptr) root = root->left;
    return root;
}

Menu* hapusMenuBST(Menu* root, string nama, bool& ketemu) {
    if (root == nullptr) return nullptr;
    if (nama < root->nama) {
        root->left = hapusMenuBST(root->left, nama, ketemu);
    } else if (nama > root->nama) {
        root->right = hapusMenuBST(root->right, nama, ketemu);
    } else {
        ketemu = true;
        if (root->left == nullptr) {
            Menu* temp = root->right;
            delete root;
            return temp;
        } else if (root->right == nullptr) {
            Menu* temp = root->left;
            delete root;
            return temp;
        }
        Menu* temp = cariMin(root->right);
        root->nama = temp->nama;
        root->kategori = temp->kategori;
        root->harga = temp->harga;
        root->tersedia = temp->tersedia;
        root->frontAntrian = temp->frontAntrian;
        root->rearAntrian = temp->rearAntrian;
        root->right = hapusMenuBST(root->right, temp->nama, ketemu);
    }
    return root;
}

int main() {
    Menu* root = nullptr;
    int pilihan, harga;
    string namaMenu, kategori, pelanggan;

    do {
        cout << "\n=== SISTEM PESANAN RESTORAN ===" << endl;
        cout << "1. Tambah Data Menu" << endl;
        cout << "2. Lihat Daftar Menu" << endl;
        cout << "3. Catat Pesanan Pelanggan" << endl;
        cout << "4. Catat Menu Tersedia Kembali" << endl;
        cout << "5. Hapus Menu" << endl;
        cout << "6. Undo Aktivitas Terakhir" << endl;
        cout << "7. Tampilkan Antrian Pelanggan" << endl;
        cout << "0. Keluar" << endl;
        cout << "Pilihan: ";
        cin >> pilihan;
        cin.ignore();

        switch (pilihan) {
            case 1:
                cout << "Masukkan Nama Menu     : "; getline(cin, namaMenu);
                cout << "Masukkan Kategori      : "; getline(cin, kategori);
                cout << "Masukkan Harga         : "; cin >> harga;
                root = insertMenu(root, namaMenu, kategori, harga);
                cout << "Menu berhasil ditambahkan" << endl;
                break;

            case 2:
                if (root == nullptr) cout << "Daftar menu kosong." << endl;
                else tampilMenu(root);
                break;

            case 3: {
                cout << "Masukkan Nama Pelanggan: "; getline(cin, pelanggan);
                cout << "Masukkan Nama Menu     : "; getline(cin, namaMenu);
                Menu* m = cariMenu(root, namaMenu);
                if (m == nullptr) {
                    cout << "Menu tidak ditemukan!" << endl;
                } else if (m->tersedia) {
                    cout << "Pesanan " << pelanggan << " berhasil dibuat" << endl;
                    m->tersedia = false;
                    pushStack("PESAN", namaMenu);
                } else {
                    cout << "Menu sedang habis, " << pelanggan << " masuk ke dalam waiting list" << endl;
                    enqueue(m, pelanggan);
                }
                break;
            }

            case 4: {
                cout << "Masukkan Nama Menu     : "; getline(cin, namaMenu);
                Menu* m = cariMenu(root, namaMenu);
                if (m == nullptr) {
                    cout << "Menu tidak ditemukan!" << endl;
                } else {
                    m->tersedia = true;
                    cout << "Menu tersedia kembali" << endl;
                    if (m->frontAntrian != nullptr) {
                        string dapat = dequeue(m);
                        cout << "Pesanan langsung diberikan kepada " << dapat << " dari waiting list" << endl;
                        m->tersedia = false;
                        pushStack("UPDATE", namaMenu, dapat);
                    } else {
                        pushStack("UPDATE", namaMenu);
                    }
                }
                break;
            }

            case 5: {
                cout << "Masukkan Nama Menu     : "; getline(cin, namaMenu);
                bool ketemu = false;
                root = hapusMenuBST(root, namaMenu, ketemu);
                if (ketemu) cout << "Menu berhasil dihapus" << endl;
                else cout << "Menu tidak ditemukan!" << endl;
                break;
            }

            case 6: {
                NodeStack* last = popStack();
                if (last == nullptr) {
                    cout << "Tidak ada aksi untuk di-undo" << endl;
                } else {
                    Menu* m = cariMenu(root, last->namaMenu);
                    if (last->jenisAksi == "PESAN") {
                        if (m != nullptr) m->tersedia = true;
                        cout << "Undo: Pesanan menu " << last->namaMenu << " dibatalkan" << endl;
                    } else if (last->jenisAksi == "UPDATE") {
                        if (m != nullptr) {
                            m->tersedia = false;
                            if (last->pelanggan != "") {
                                pushFrontAntrian(m, last->pelanggan);
                            }
                        }
                        cout << "Undo: Menu " << last->namaMenu << " kembali habis" << endl;
                    }
                    delete last;
                }
                break;
            }

            case 7: {
                cout << "Masukkan Nama Menu     : "; getline(cin, namaMenu);
                Menu* m = cariMenu(root, namaMenu);
                if (m == nullptr) {
                    cout << "Menu tidak ditemukan!" << endl;
                } else if (m->frontAntrian == nullptr) {
                    cout << "Antrian kosong" << endl;
                } else {
                    NodeQueue* bantu = m->frontAntrian;
                    while (bantu != nullptr) {
                        cout << bantu->pelanggan << " menunggu menu " << namaMenu << endl;
                        bantu = bantu->next;
                    }
                }
                break;
            }
        }
    } while (pilihan != 0);

    return 0;
}