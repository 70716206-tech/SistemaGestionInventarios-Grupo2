#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <ctime>

using namespace std;

// =============================================
// ESTRUCTURAS DE DATOS
// =============================================

struct Categoria {
    int id;
    string nombre;
    string descripcion;
};

struct Producto {
    int id;
    string nombre;
    string descripcion;
    double precio;
    int stock;
    int idCategoria;
    string fechaIngreso;
};

// =============================================
// CLASE PRINCIPAL DEL SISTEMA
// =============================================

class Inventario {
private:
    vector<Producto> productos;
    vector<Categoria> categorias;
    int nextProductoId;
    int nextCategoriaId;
    
    // Archivos para persistencia de datos
    const string archivoProductos = "data/productos.txt";
    const string archivoCategorias = "data/categorias.txt";
    const string archivoLog = "data/log_sistema.txt";

public:
    // Constructor - inicializa IDs y carga datos existentes
    Inventario() : nextProductoId(1), nextCategoriaId(1) {
        cargarDatos();
        registrarLog("Sistema de inventario iniciado");
    }
    
    // =============================================
    // METODOS PARA GESTION DE CATEGORIAS
    // =============================================
    
    void agregarCategoria() {
        Categoria cat;
        cat.id = nextCategoriaId++;
        
        cout << "\n" << string(50, '=') << endl;
        cout << "         AGREGAR NUEVA CATEGORIA" << endl;
        cout << string(50, '=') << endl;
        
        cout << "Nombre de la categoria: ";
        cin.ignore();
        getline(cin, cat.nombre);
        
        cout << "Descripcion: ";
        getline(cin, cat.descripcion);
        
        categorias.push_back(cat);
        guardarCategorias();
        
        cout << "\nCATEGORIA AGREGADA EXITOSAMENTE!" << endl;
        cout << "ID: " << cat.id << " | Nombre: " << cat.nombre << endl;
        
        registrarLog("Categoria agregada: " + cat.nombre);
    }
    
    void listarCategorias() {
        cout << "\n" << string(50, '=') << endl;
        cout << "           LISTA DE CATEGORIAS" << endl;
        cout << string(50, '=') << endl;
        
        if (categorias.empty()) {
            cout << "No hay categorias registradas en el sistema." << endl;
            return;
        }
        
        cout << left << setw(5) << "ID" << setw(20) << "NOMBRE" 
             << setw(25) << "DESCRIPCION" << endl;
        cout << string(60, '-') << endl;
        
        for (const auto& cat : categorias) {
            cout << left << setw(5) << cat.id << setw(20) << cat.nombre 
                 << setw(25) << cat.descripcion << endl;
        }
        
        cout << "\nTotal de categorias: " << categorias.size() << endl;
    }
    
    // =============================================
    // METODOS PARA GESTION DE PRODUCTOS
    // =============================================
    
    void agregarProducto() {
        Producto prod;
        prod.id = nextProductoId++;
        
        cout << "\n" << string(50, '=') << endl;
        cout << "          AGREGAR NUEVO PRODUCTO" << endl;
        cout << string(50, '=') << endl;
        
        cout << "Nombre del producto: ";
        cin.ignore();
        getline(cin, prod.nombre);
        
        cout << "Descripcion: ";
        getline(cin, prod.descripcion);
        
        cout << "Precio: S/.";
        while (!(cin >> prod.precio) || prod.precio < 0) {
            cout << "Precio invalido. Ingrese un valor positivo: S/.";
            cin.clear();
            cin.ignore(10000, '\n');
        }
        
        cout << "Stock inicial: ";
        while (!(cin >> prod.stock) || prod.stock < 0) {
            cout << "Stock invalido. Ingrese un valor positivo: ";
            cin.clear();
            cin.ignore(10000, '\n');
        }
        
        // Mostrar categorías disponibles
        if (!categorias.empty()) {
            cout << "\nCategorias disponibles:" << endl;
            for (const auto& cat : categorias) {
                cout << "   " << cat.id << ". " << cat.nombre << endl;
            }
        }
        
        cout << "ID de categoria: ";
        while (!(cin >> prod.idCategoria) || !existeCategoria(prod.idCategoria)) {
            cout << "Categoria no existe. Ingrese un ID valido: ";
            cin.clear();
            cin.ignore(10000, '\n');
        }
        
        cout << "Fecha de ingreso (DD/MM/AAAA): ";
        cin >> prod.fechaIngreso;
        
        productos.push_back(prod);
        guardarProductos();
        
        cout << "\nPRODUCTO AGREGADO EXITOSAMENTE!" << endl;
        cout << "ID: " << prod.id << " | Nombre: " << prod.nombre << endl;
        cout << "Precio: S/." << prod.precio << " | Stock: " << prod.stock << endl;
        
        registrarLog("Producto agregado: " + prod.nombre);
    }
    
    void listarProductos() {
        cout << "\n" << string(70, '=') << endl;
        cout << "                     LISTA DE PRODUCTOS" << endl;
        cout << string(70, '=') << endl;
        
        if (productos.empty()) {
            cout << "No hay productos registrados en el sistema." << endl;
            return;
        }
        
        cout << left << setw(5) << "ID" << setw(20) << "NOMBRE" 
             << setw(10) << "PRECIO" << setw(8) << "STOCK" 
             << setw(15) << "CATEGORIA" << setw(12) << "FECHA ING." << endl;
        cout << string(70, '-') << endl;
        
        for (const auto& prod : productos) {
            string nombreCategoria = obtenerNombreCategoria(prod.idCategoria);
            cout << left << setw(5) << prod.id 
                 << setw(20) << (prod.nombre.length() > 18 ? prod.nombre.substr(0, 15) + "..." : prod.nombre)
                 << setw(10) << fixed << setprecision(2) << prod.precio
                 << setw(8) << prod.stock
                 << setw(15) << (nombreCategoria.length() > 12 ? nombreCategoria.substr(0, 10) + "..." : nombreCategoria)
                 << setw(12) << prod.fechaIngreso << endl;
        }
        
        cout << "\nTotal de productos: " << productos.size() << endl;
        
        // Calcular valor total del inventario
        double valorTotal = 0;
        for (const auto& prod : productos) {
            valorTotal += prod.precio * prod.stock;
        }
        cout << "Valor total del inventario: S/." << fixed << setprecision(2) << valorTotal << endl;
    }
    
    void buscarProducto() {
        int id;
        cout << "\n" << string(50, '=') << endl;
        cout << "           BUSCAR PRODUCTO POR ID" << endl;
        cout << string(50, '=') << endl;
        
        cout << "Ingrese el ID del producto: ";
        cin >> id;
        
        for (const auto& prod : productos) {
            if (prod.id == id) {
                mostrarDetallesProducto(prod);
                registrarLog("Busqueda de producto ID: " + to_string(id));
                return;
            }
        }
        cout << "Producto con ID " << id << " no encontrado." << endl;
    }
    
    void buscarProductoPorNombre() {
        string nombreBuscado;
        cout << "\n" << string(50, '=') << endl;
        cout << "        BUSCAR PRODUCTO POR NOMBRE" << endl;
        cout << string(50, '=') << endl;
        
        cout << "Ingrese el nombre o parte del nombre: ";
        cin.ignore();
        getline(cin, nombreBuscado);
        
        // Convertir a minúsculas para búsqueda case-insensitive
        transform(nombreBuscado.begin(), nombreBuscado.end(), nombreBuscado.begin(), ::tolower);
        
        vector<Producto> resultados;
        for (const auto& prod : productos) {
            string nombreLower = prod.nombre;
            transform(nombreLower.begin(), nombreLower.end(), nombreLower.begin(), ::tolower);
            
            if (nombreLower.find(nombreBuscado) != string::npos) {
                resultados.push_back(prod);
            }
        }
        
        if (resultados.empty()) {
            cout << "No se encontraron productos con: '" << nombreBuscado << "'" << endl;
            return;
        }
        
        cout << "\nResultados de la busqueda (" << resultados.size() << " encontrados):" << endl;
        cout << string(60, '-') << endl;
        
        for (const auto& prod : resultados) {
            cout << "ID: " << prod.id << " | Nombre: " << prod.nombre 
                 << " | Stock: " << prod.stock << " | Precio: S/." << prod.precio << endl;
        }
        
        registrarLog("Busqueda por nombre: " + nombreBuscado);
    }
    
    void actualizarStock() {
        int id, nuevoStock;
        cout << "\n" << string(50, '=') << endl;
        cout << "           ACTUALIZAR STOCK" << endl;
        cout << string(50, '=') << endl;
        
        cout << "ID del producto: ";
        cin >> id;
        
        // Buscar producto
        auto it = find_if(productos.begin(), productos.end(), 
                         [id](const Producto& p) { return p.id == id; });
        
        if (it == productos.end()) {
            cout << "Producto con ID " << id << " no encontrado." << endl;
            return;
        }
        
        cout << "Producto: " << it->nombre << endl;
        cout << "Stock actual: " << it->stock << endl;
        cout << "Nuevo stock: ";
        cin >> nuevoStock;
        
        if (nuevoStock < 0) {
            cout << "El stock no puede ser negativo." << endl;
            return;
        }
        
        int stockAnterior = it->stock;
        it->stock = nuevoStock;
        guardarProductos();
        
        cout << "\nSTOCK ACTUALIZADO EXITOSAMENTE!" << endl;
        cout << "Producto: " << it->nombre << endl;
        cout << "Stock anterior: " << stockAnterior << " -> Nuevo stock: " << nuevoStock << endl;
        
        registrarLog("Stock actualizado - Producto: " + it->nombre + 
                    " (" + to_string(stockAnterior) + "->" + to_string(nuevoStock) + ")");
    }
    
    void productosStockBajo() {
        int limite;
        cout << "\n" << string(50, '=') << endl;
        cout << "        PRODUCTOS CON STOCK BAJO" << endl;
        cout << string(50, '=') << endl;
        
        cout << "Ingrese el limite de stock para alerta: ";
        cin >> limite;
        
        vector<Producto> productosBajos;
        copy_if(productos.begin(), productos.end(), back_inserter(productosBajos),
               [limite](const Producto& p) { return p.stock < limite; });
        
        if (productosBajos.empty()) {
            cout << "No hay productos con stock menor a " << limite << endl;
            return;
        }
        
        cout << "\nALERTA: " << productosBajos.size() << " productos con stock bajo:" << endl;
        cout << string(70, '-') << endl;
        cout << left << setw(5) << "ID" << setw(20) << "NOMBRE" 
             << setw(10) << "STOCK" << setw(10) << "LIMITE" << setw(15) << "CATEGORIA" << endl;
        cout << string(70, '-') << endl;
        
        for (const auto& prod : productosBajos) {
            string nombreCategoria = obtenerNombreCategoria(prod.idCategoria);
            cout << left << setw(5) << prod.id 
                 << setw(20) << prod.nombre
                 << setw(10) << prod.stock
                 << setw(10) << limite
                 << setw(15) << nombreCategoria << endl;
        }
        
        registrarLog("Reporte stock bajo generado - Limite: " + to_string(limite));
    }
    
    void generarReporteGeneral() {
        cout << "\n" << string(60, '=') << endl;
        cout << "              REPORTE GENERAL DEL INVENTARIO" << endl;
        cout << string(60, '=') << endl;
        
        if (productos.empty()) {
            cout << "No hay productos en el inventario." << endl;
            return;
        }
        
        // Estadísticas generales
        int totalProductos = productos.size();
        int totalStock = 0;
        double valorTotal = 0;
        Producto productoMasCaro = productos[0];
        Producto productoMenorStock = productos[0];
        
        for (const auto& prod : productos) {
            totalStock += prod.stock;
            valorTotal += prod.precio * prod.stock;
            
            if (prod.precio > productoMasCaro.precio) {
                productoMasCaro = prod;
            }
            if (prod.stock < productoMenorStock.stock) {
                productoMenorStock = prod;
            }
        }
        
        cout << "ESTADISTICAS GENERALES:" << endl;
        cout << "- Total de productos: " << totalProductos << endl;
        cout << "- Total de unidades en stock: " << totalStock << endl;
        cout << "- Valor total del inventario: S/." << fixed << setprecision(2) << valorTotal << endl;
        cout << "- Precio promedio: S/." << fixed << setprecision(2) << (valorTotal / totalStock) << endl;
        
        cout << "\nPRODUCTOS DESTACADOS:" << endl;
        cout << "- Producto mas caro: " << productoMasCaro.nombre << " (S/." << productoMasCaro.precio << ")" << endl;
        cout << "- Producto con menor stock: " << productoMenorStock.nombre << " (" << productoMenorStock.stock << " unidades)" << endl;
        
        // Productos por categoría
        cout << "\nDISTRIBUCION POR CATEGORIA:" << endl;
        for (const auto& cat : categorias) {
            int count = count_if(productos.begin(), productos.end(),
                               [cat](const Producto& p) { return p.idCategoria == cat.id; });
            if (count > 0) {
                cout << "- " << cat.nombre << ": " << count << " productos" << endl;
            }
        }
        
        registrarLog("Reporte general generado");
    }

private:
    // =============================================
    // METODOS PRIVADOS DE APOYO
    // =============================================
    
    bool existeCategoria(int idCategoria) {
        return any_of(categorias.begin(), categorias.end(),
                     [idCategoria](const Categoria& c) { return c.id == idCategoria; });
    }
    
    string obtenerNombreCategoria(int idCategoria) {
        for (const auto& cat : categorias) {
            if (cat.id == idCategoria) {
                return cat.nombre;
            }
        }
        return "Sin categoria";
    }

    void mostrarDetallesProducto(const Producto& prod) {
        cout << "\n" << string(50, '=') << endl;
        cout << "          DETALLES DEL PRODUCTO" << endl;
        cout << string(50, '=') << endl;
        
        cout << "ID: " << prod.id << endl;
        cout << "Nombre: " << prod.nombre << endl;
        cout << "Descripcion: " << prod.descripcion << endl;
        cout << "Precio: S/." << fixed << setprecision(2) << prod.precio << endl;
        cout << "Stock: " << prod.stock << " unidades" << endl;
        cout << "Categoria: " << obtenerNombreCategoria(prod.idCategoria) << endl;
        cout << "Fecha de ingreso: " << prod.fechaIngreso << endl;
        cout << "Valor total: S/." << fixed << setprecision(2) << (prod.precio * prod.stock) << endl;
        
        // Indicador de stock
        if (prod.stock == 0) {
            cout << "ESTADO: AGOTADO" << endl;
        } else if (prod.stock < 5) {
            cout << "ESTADO: STOCK BAJO" << endl;
        } else {
            cout << "ESTADO: STOCK NORMAL" << endl;
        }
    }
    
    void registrarLog(const string& mensaje) {
        ofstream log(archivoLog, ios::app);
        if (log.is_open()) {
            // Obtener fecha y hora actual
            time_t now = time(0);
            tm* timeinfo = localtime(&now);
            char buffer[80];
            strftime(buffer, 80, "[%d/%m/%Y %H:%M:%S]", timeinfo);
            
            log << buffer << " " << mensaje << endl;
            log.close();
        }
    }
    
    // =============================================
    // METODOS DE PERSISTENCIA
    // =============================================
    
    void guardarProductos() {
        ofstream archivo(archivoProductos);
        if (archivo.is_open()) {
            for (const auto& prod : productos) {
                archivo << prod.id << "," << prod.nombre << "," << prod.descripcion << ","
                       << fixed << setprecision(2) << prod.precio << "," << prod.stock << "," 
                       << prod.idCategoria << "," << prod.fechaIngreso << endl;
            }
            archivo.close();
        }
    }
    
    void guardarCategorias() {
        ofstream archivo(archivoCategorias);
        if (archivo.is_open()) {
            for (const auto& cat : categorias) {
                archivo << cat.id << "," << cat.nombre << "," << cat.descripcion << endl;
            }
            archivo.close();
        }
    }
    
    void cargarDatos() {
        cargarCategorias();
        cargarProductos();
    }
    
    void cargarProductos() {
        ifstream archivo(archivoProductos);
        if (archivo.is_open()) {
            string linea;
            while (getline(archivo, linea)) {
                Producto prod;
                size_t pos = 0;
                string token;
                int campo = 0;
                
                while ((pos = linea.find(',')) != string::npos) {
                    token = linea.substr(0, pos);
                    switch (campo) {
                        case 0: prod.id = stoi(token); break;
                        case 1: prod.nombre = token; break;
                        case 2: prod.descripcion = token; break;
                        case 3: prod.precio = stod(token); break;
                        case 4: prod.stock = stoi(token); break;
                        case 5: prod.idCategoria = stoi(token); break;
                    }
                    linea.erase(0, pos + 1);
                    campo++;
                }
                prod.fechaIngreso = linea;
                productos.push_back(prod);
                if (prod.id >= nextProductoId) nextProductoId = prod.id + 1;
            }
            archivo.close();
        }
    }
    
    void cargarCategorias() {
        ifstream archivo(archivoCategorias);
        if (archivo.is_open()) {
            string linea;
            while (getline(archivo, linea)) {
                Categoria cat;
                size_t pos = 0;
                string token;
                int campo = 0;
                
                while ((pos = linea.find(',')) != string::npos) {
                    token = linea.substr(0, pos);
                    switch (campo) {
                        case 0: cat.id = stoi(token); break;
                        case 1: cat.nombre = token; break;
                        case 2: cat.descripcion = token; break;
                    }
                    linea.erase(0, pos + 1);
                    campo++;
                }
                categorias.push_back(cat);
                if (cat.id >= nextCategoriaId) nextCategoriaId = cat.id + 1;
            }
            archivo.close();
        }
    }
};

// =============================================
// FUNCIONES DE INTERFAZ DE USUARIO
// =============================================

void mostrarMenuPrincipal() {
    cout << "\n" << string(60, '=') << endl;
    cout << "        SISTEMA DE GESTION DE INVENTARIOS" << endl;
    cout << string(60, '=') << endl;
    cout << "1.  Agregar Categoria" << endl;
    cout << "2.  Listar Categorias" << endl;
    cout << "3.  Agregar Producto" << endl;
    cout << "4.  Listar Productos" << endl;
    cout << "5.  Buscar Producto por ID" << endl;
    cout << "6.  Buscar Producto por Nombre" << endl;
    cout << "7.  Actualizar Stock" << endl;
    cout << "8.  Productos con Stock Bajo" << endl;
    cout << "9.  Generar Reporte General" << endl;
    cout << "10. Salir del Sistema" << endl;
    cout << string(60, '-') << endl;
    cout << "Seleccione una opcion (1-10): ";
}

void mostrarEncabezado() {
    cout << string(70, '=') << endl;
    cout << "                SISTEMA DE GESTION DE INVENTARIOS v2.0" << endl;
    cout << "                  Desarrollado por: Grupo X" << endl;
    cout << "                  Curso: Construccion de Software" << endl;
    cout << string(70, '=') << endl;
}

// =============================================
// FUNCIÓN PRINCIPAL
// =============================================

int main() {
    Inventario inventario;
    int opcion;
    
    // Configurar decimales
    cout << fixed << setprecision(2);
    
    mostrarEncabezado();
    cout << "\nBienvenido al Sistema de Gestion de Inventarios!" << endl;
    cout << "   Cargando datos existentes..." << endl;
    
    do {
        mostrarMenuPrincipal();
        cin >> opcion;
        
        switch (opcion) {
            case 1: inventario.agregarCategoria(); break;
            case 2: inventario.listarCategorias(); break;
            case 3: inventario.agregarProducto(); break;
            case 4: inventario.listarProductos(); break;
            case 5: inventario.buscarProducto(); break;
            case 6: inventario.buscarProductoPorNombre(); break;
            case 7: inventario.actualizarStock(); break;
            case 8: inventario.productosStockBajo(); break;
            case 9: inventario.generarReporteGeneral(); break;
            case 10: cout << "Saliendo del sistema... ¡Hasta pronto!" << endl; break;
            default: cout << "Opcion no valida. Intente nuevamente." << endl;
        }
        
        if (opcion != 10) {
            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            cin.get();
        }
        
    } while (opcion != 10);
    
    return 0;
}