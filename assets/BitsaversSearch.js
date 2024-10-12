function LoadDataIntoTable(data) {
    let btdata = [];
    for (const section in data) {
        const sect = data[section];
        for (const index in sect) {
            const entry = sect[index];
            const href = section + '/' + (entry[1] == '' ? '' : entry[1] + '/') + entry[2];
            btdata.push([entry[2], section, entry[1] == '' ? section : entry[1], href]);
        }
    }
    $('#bitsaversSearchTable').DataTable({
        data: btdata,
        autoWidth: false,
        columnDefs: [
            {
                targets: 3,
                render: function (data) {
                    return '<button class=\"btn btn-primary btn-sm\" onclick=\'dl(\"https://bitsavers.org/' + data + '\")\'>Download</button>'
                }
            }
        ]
    });
}

$.ajax({
    type: "GET",
    url: 'data.json',
    dataType: "json",
    success: function (data) {
        LoadDataIntoTable(data);
    },
    error: function () {
        alert("Error while inserting data");
    }
});