def getOrderedHorizonHalfEdges(point_node, D):
	# Trouver les edges autour du groupe de faces en conflit avec point_node
	# Critere: Inclure un half-edge si son twin est sur une face non en conflit (invisible)

	horizon = []

	# Trouver un premier half-edge faisant partie de l'horizon
	for face in point_node.conflits:
		horizon_half_edge = getHalfEdgeTouchingHorizon(face, point_node)
		if horizon_half_edge is not None:
			horizon.append(horizon_half_edge)
			break

	# Trouver les half-edge suivants sur l'horizon
	# A partir d'un half-edge, verifier les half-edges touchant son extremite
	half_edge = findNextHorizonHalfEdge(horizon[0], point_node)
	while half_edge != horizon[0]:
		horizon.append(half_edge)
		half_edge.findNextHorizonHalfEdge(half_edge, point_node)

	return horizon

def findNextHorizonHalfEdge(half_edge_from, point_node):
	half_edge = half_edge_from.next
	continuer = True
	while continuer:
		# Verifier si le half-edge borde la region visible
		if half_edge.twin.face not in point_node.conflits:
			return half_edge
		# Suite de la boucle
		half_edge = half_edge.twin.next
		continuer = half_edge != half_edge_from

	return None # Erreur! Le half-edge fourni ne borde pas l'horizon!

def getHalfEdgeTouchingHorizon(face, point_node):
	# Boucler sur les half-edges de la face
	premier_half_edge = face.an_edge
	half_edge = premier_half_edge
	continuer = True 
	while continuer:
		# Si twin fait partie d'une face invisible, on a trouve
		if half_edge.twin.face not in point_node.conflits:
			return half_edge
		# Suite de la boucle
		half_edge = half_edge.next
		continuer = half_edge != premier_half_edge
	# Si on se rend ici, la face n'est pas a l'interieur de l'horizon
	return None